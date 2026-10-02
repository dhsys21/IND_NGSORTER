#ifndef TeachingReviewH
#define TeachingReviewH

#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <System.IniFiles.hpp>
#include <Winapi.Windows.hpp>
#include <math.h>
#include <memory>

// The established tray teaching scale is 1,000 command units per mm.
inline bool ValidTeachingTolerance(double mm)
{
    return mm > 0.0 && mm < HUGE_VAL;
}

inline __int64 DefaultTeachingCoordinate(int origin, int channel, bool xAxis)
{
    static const int columns[4] = {0, 230000, 490000, 720000};
    if(channel < 1 || channel > 96) throw Exception("Invalid teaching channel.");
    int group = (channel - 1) / 12;
    return (__int64)origin + (xAxis ? columns[group / 2] :
        (group % 2) * 590000 + ((channel - 1) % 12) * 45000);
}

inline double TeachingDifferenceMm(__int64 actual, __int64 expected)
{
    return (actual - expected) / 1000.0;
}

inline bool TeachingOutsideTolerance(__int64 actual, __int64 expected, double tolerance)
{
    if(!ValidTeachingTolerance(tolerance)) throw Exception("Invalid teaching XY tolerance.");
    return fabs(TeachingDifferenceMm(actual, expected)) >= tolerance;
}

inline UnicodeString BackupTeachingFile(const UnicodeString &file, TDateTime stamp)
{
    if(!FileExists(file)) return ""; // First save has no previous teaching file.
    UnicodeString folder = ExtractFilePath(file) + "TeachingBackup\\";
    if(!ForceDirectories(folder)) throw Exception("Cannot create teaching backup folder.");
    UnicodeString prefix = folder + ChangeFileExt(ExtractFileName(file), "") + "_" +
        stamp.FormatString("yyyymmdd_hhnnss_zzz");
    for(int attempt = 0; attempt < 10000; ++attempt){
        UnicodeString backup = prefix + (attempt ? "_" + IntToStr(attempt) : UnicodeString("")) +
            ExtractFileExt(file);
        if(CopyFileW(file.c_str(), backup.c_str(), TRUE)) return backup;
        DWORD error = GetLastError();
        if(error != ERROR_FILE_EXISTS && error != ERROR_ALREADY_EXISTS)
            throw Exception("Teaching backup failed: " + SysErrorMessage(error));
    }
    throw Exception("Cannot allocate a unique teaching backup filename.");
}

inline void AppendTeachingAudit(const UnicodeString &folder, TDateTime stamp,
    const UnicodeString &entry)
{
    if(!ForceDirectories(folder)) throw Exception("Cannot create teaching log folder.");
    UnicodeString path = IncludeTrailingPathDelimiter(folder) + "TEACHING_" +
        stamp.FormatString("yyyymmdd") + ".log";
    HANDLE file = CreateFileW(path.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(file == INVALID_HANDLE_VALUE) throw Exception("Cannot open teaching log: " + path);
    UTF8String bytes(stamp.FormatString("yyyy-mm-dd hh:nn:ss.zzz") + " " + entry + "\r\n");
    DWORD written = 0;
    bool ok = WriteFile(file, bytes.c_str(), bytes.Length(), &written, NULL) != 0;
    if(ok && written == (DWORD)bytes.Length()) ok = FlushFileBuffers(file) != 0;
    else ok = false;
    CloseHandle(file);
    if(!ok) throw Exception("Cannot write teaching log: " + path);
}

inline UnicodeString TeachingValueAudit(const UnicodeString &section, const UnicodeString &key,
    const UnicodeString &previous, const UnicodeString &next)
{
    int oldValue = 0, newValue = 0;
    UnicodeString delta = "N/A";
    bool coordinate = section == "Source" || section == "Target" || key == "SourceZ" || key == "TargetZ";
    if(TryStrToInt(previous, oldValue) && TryStrToInt(next, newValue)){
        __int64 difference = (__int64)newValue - oldValue;
        delta = IntToStr(difference);
        if(coordinate) delta += " (" + FormatFloat("+0.000;-0.000;0.000", difference / 1000.0) + " mm)";
    }
    return section + "." + key + " Previous=" + previous + " New=" + next + " Difference=" + delta;
}

inline UnicodeString CommitReviewedTeachingFile(const UnicodeString &file,
    const UnicodeString &temporary, const UnicodeString &logFolder, TDateTime stamp,
    const UnicodeString &actor, bool forced, double tolerance, const UnicodeString &deviations,
    UnicodeString &completionLogWarning)
{
    completionLogWarning = "";
    UnicodeString backup = BackupTeachingFile(file, stamp);
    TGUID guid;
    if(CreateGUID(guid) != 0) throw Exception("Cannot create teaching audit ID.");
    UnicodeString id = System::Sysutils::GUIDToString(guid);
    UnicodeString review = "[TEACHING] SAVE_PREPARED ID=" + id + " User=" + actor +
        " Forced=" + (forced ? "YES" : "NO") + " XYToleranceMm=" + FloatToStr(tolerance) +
        " Backup=" + (backup.IsEmpty() ? UnicodeString("NONE (first save)") : backup);
    std::auto_ptr<TMemIniFile> next(new TMemIniFile(temporary));
    std::auto_ptr<TMemIniFile> previous;
    if(!backup.IsEmpty()) previous.reset(new TMemIniFile(backup));
    std::auto_ptr<TStringList> sections(new TStringList());
    std::auto_ptr<TStringList> keys(new TStringList());
    next->ReadSections(sections.get());
    for(int s = 0; s < sections->Count; ++s){
        keys->Clear();
        next->ReadSection(sections->Strings[s], keys.get());
        for(int k = 0; k < keys->Count; ++k){
            UnicodeString section = sections->Strings[s], key = keys->Strings[k];
            UnicodeString oldValue = previous.get() ? previous->ReadString(section, key, "<missing>") : UnicodeString("<none>");
            review += "\r\n  " + TeachingValueAudit(section, key, oldValue, next->ReadString(section, key, ""));
        }
    }
    if(!deviations.IsEmpty()) review += "\r\n  Drawing deviations (mm):\r\n" + deviations;
    // Preserve the previous file AND a durable full audit before replacing live teaching.
    AppendTeachingAudit(logFolder, stamp, review);
    if(!MoveFileExW(temporary.c_str(), file.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)){
        DWORD error = GetLastError();
        try{ AppendTeachingAudit(logFolder, stamp, "[TEACHING] SAVE_FAILED ID=" + id + " Error=" + SysErrorMessage(error)); }
        catch(...){ }
        throw Exception("Teaching file replacement failed: " + SysErrorMessage(error));
    }
    try{ AppendTeachingAudit(logFolder, stamp, "[TEACHING] SAVE_COMPLETED ID=" + id); }
    catch(Exception &error){
        // The new file is committed: still apply it, but report the missing completion marker.
        completionLogWarning = error.Message;
    }
    return backup;
}
#endif
