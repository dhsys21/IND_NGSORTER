$ErrorActionPreference = 'Stop'
# The original .in fixture records the pre-fix reproduction for the review.
# The active test now verifies the corrected behavior against production bodies.
& (Join-Path $PSScriptRoot '../ProductionHardening/test_hardening.ps1')
