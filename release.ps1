
do
{
    try {
    [ValidatePattern('^(\d+\.)?(\d+\.)?(\d+\.)?(\*|\d+)$')]$Ver = Read-Host -Prompt "Enter a Version Number (X.X.X.X)" 
    } catch {}
} until ($?)


Write-Host Packing $Ver to uLister_$Ver.zip
Compress-Archive -Path release\* uLister_$Ver.zip