using System;
using System.IO;
using System.Runtime.InteropServices;
class WordExport {
 [STAThread] static int Main(string[] args) {
  dynamic app=null, doc=null; bool original=true;
  try {
   app=Activator.CreateInstance(Type.GetTypeFromProgID("Word.Application"));
   app.Visible=false; app.DisplayAlerts=0;
   original=app.Options.AlertIfNotDefault; app.Options.AlertIfNotDefault=false;
   Console.WriteLine("Opening");
   doc=app.Documents.Open(Path.GetFullPath(args[0]), ReadOnly:args.Length>2, AddToRecentFiles:false);
   Console.WriteLine("Document: "+doc.Name);
   if(args.Length<3){doc.Fields.Update(); doc.Repaginate(); doc.Save();}
   doc.ExportAsFixedFormat(Path.GetFullPath(args[1]),17);
   Console.WriteLine("Pages: "+doc.ComputeStatistics(2));
   return 0;
  } catch(Exception e){Console.WriteLine(e);return 1;}
  finally {
   if(doc!=null){try{doc.Close(0);Marshal.ReleaseComObject(doc);}catch{}}
   if(app!=null){try{app.Options.AlertIfNotDefault=original;app.Quit();Marshal.ReleaseComObject(app);}catch{}}
  }
 }
}
