param(
    [switch]$Coverage,
    [switch]$Cross
)

$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'

$unbound = $args
if ($unbound.Count -gt 0) {
    [Console]::Error.WriteLine('Unknown flag(s): ' + ($unbound -join ' '))
    [Console]::Error.WriteLine('Usage: install-windows.ps1 [-Coverage] [-Cross]')
    exit 1
}

if ($Cross) {
    Write-Host 'Warning: -Cross is not applicable on Windows for this project; continuing.'
}

if ($Coverage) {
    Write-Host 'Warning: -Coverage is not applicable on Windows for this project; continuing.'
}

$principal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
$isAdmin = $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Host 'Warning: shell is not elevated. Chocolatey and winget may require an elevated (admin) shell.'
}

Write-Host 'Detecting package manager...'
$choco = Get-Command choco -ErrorAction SilentlyContinue
if ($choco) {
    Write-Host 'Found Chocolatey; installing build toolchain...'
    choco install -y --no-progress cmake ninja git ccache openssl 7zip
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Warning: choco install exited with code $LASTEXITCODE; continuing anyway."
    }
} else {
    $winget = Get-Command winget -ErrorAction SilentlyContinue
    if ($winget) {
        Write-Host 'Chocolatey not found on PATH; falling back to winget (best-effort)...'
        $wingetIds = @('Kitware.CMake', 'Ninja-build.Ninja', 'Git.Git', 'ccache.ccache', 'ShiningLight.OpenSSL.Light', '7zip.7zip')
        foreach ($id in $wingetIds) {
            try {
                winget install --id $id --silent --accept-package-agreements --accept-source-agreements
                if ($LASTEXITCODE -ne 0) {
                    throw "winget install exited with code $LASTEXITCODE"
                }
                Write-Host "Installed: $id"
            } catch {
                Write-Host "Warning: winget install failed for '$id' (possibly an invalid package id); continuing with the remaining packages."
            }
        }
    } else {
        [Console]::Error.WriteLine('Neither Chocolatey nor winget is available on PATH.')
        [Console]::Error.WriteLine('Run the following commands from an elevated (admin) PowerShell:')
        [Console]::Error.WriteLine('')
        [Console]::Error.WriteLine('Option 1: install the Chocolatey bootstrapper, then:')
        [Console]::Error.WriteLine('  Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = 3072; iex ((New-Object System.Net.WebClient).DownloadString(''https://community.chocolatey.org/install.ps1''))')
        [Console]::Error.WriteLine('  choco install -y --no-progress cmake ninja git ccache openssl 7zip')
        [Console]::Error.WriteLine('')
        [Console]::Error.WriteLine('Option 2: install each package with winget:')
        [Console]::Error.WriteLine('  winget install --id Kitware.CMake --silent')
        [Console]::Error.WriteLine('  winget install --id Ninja-build.Ninja --silent')
        [Console]::Error.WriteLine('  winget install --id Git.Git --silent')
        [Console]::Error.WriteLine('  winget install --id ccache.ccache --silent')
        [Console]::Error.WriteLine('  winget install --id ShiningLight.OpenSSL.Light --silent')
        [Console]::Error.WriteLine('  winget install --id 7zip.7zip --silent')
        [Console]::Error.WriteLine('')
        [Console]::Error.WriteLine('Then re-run this script.')
        exit 1
    }
}

Write-Host 'Refreshing PATH for the current process...'
$machinePath = [Environment]::GetEnvironmentVariable('Path', 'Machine')
$userPath = [Environment]::GetEnvironmentVariable('Path', 'User')
$env:Path = ($machinePath + ';' + $userPath + ';C:\ProgramData\chocolatey\bin;C:\Program Files\CMake\bin;C:\Program Files\Ninja\bin')

Write-Host 'Installed toolchain versions:'
foreach ($tool in @('cmake', 'ninja', 'git', 'ccache')) {
    if (Get-Command $tool -ErrorAction SilentlyContinue) {
        & $tool --version
    } else {
        Write-Host "$tool --version failed: $tool not found on PATH."
    }
}
