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
    $required = @()
    foreach ($package in @(
        @{ Name = 'ccache'; Command = 'ccache' },
        @{ Name = 'curl'; Command = 'curl' },
        @{ Name = 'gnupg'; Command = 'gpg' }
    )) {
        if (-not (Get-Command $package.Command -ErrorAction SilentlyContinue)) {
            $required += $package.Name
        }
    }

    if ($required.Count -gt 0) {
        Write-Host "Installing missing packages via Chocolatey: $($required -join ', ')..."
        choco install -y --no-progress $required
        if ($LASTEXITCODE -ne 0) {
            Write-Host "Warning: choco install exited with code $LASTEXITCODE; continuing anyway."
        }
    } else {
        Write-Host 'All required Chocolatey tools are already available on PATH.'
    }
} else {
    $winget = Get-Command winget -ErrorAction SilentlyContinue
    if ($winget) {
        Write-Host 'Chocolatey not found on PATH; falling back to winget (best-effort)...'
        $wingetIds = @('Kitware.CMake', 'Ninja-build.Ninja', 'Git.Git', 'ccache.ccache', 'ShiningLight.OpenSSL.Light', '7zip.7zip', 'cURL.cURL', 'GnuPG.GnuPG')
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
        [Console]::Error.WriteLine('  choco install -y --no-progress cmake ninja git ccache 7zip curl gnupg')
        [Console]::Error.WriteLine('  Run this script to install OpenSSL and zlib through vcpkg.')
        [Console]::Error.WriteLine('')
        [Console]::Error.WriteLine('Option 2: install each package with winget:')
        [Console]::Error.WriteLine('  winget install --id Kitware.CMake --silent')
        [Console]::Error.WriteLine('  winget install --id Ninja-build.Ninja --silent')
        [Console]::Error.WriteLine('  winget install --id Git.Git --silent')
        [Console]::Error.WriteLine('  winget install --id ccache.ccache --silent')
        [Console]::Error.WriteLine('  winget install --id ShiningLight.OpenSSL.Light --silent')
        [Console]::Error.WriteLine('  winget install --id 7zip.7zip --silent')
        [Console]::Error.WriteLine('  winget install --id cURL.cURL --silent')
        [Console]::Error.WriteLine('  winget install --id GnuPG.GnuPG --silent')
        [Console]::Error.WriteLine('')
        [Console]::Error.WriteLine('Then re-run this script.')
        exit 1
    }
}

$vcpkgRoot = if ($env:VCPKG_ROOT) { $env:VCPKG_ROOT } else { 'C:\vcpkg' }
$vcpkg = Join-Path $vcpkgRoot 'vcpkg.exe'
if (-not (Test-Path $vcpkg)) {
    throw "MSVC OpenSSL and zlib require vcpkg, but it was not found at '$vcpkg'."
}

$manifestRoot = Join-Path ([System.IO.Path]::GetTempPath()) ("lis-vcpkg-" + [guid]::NewGuid().ToString())
New-Item -ItemType Directory -Path $manifestRoot | Out-Null
@'
{
  "name": "lis-windows-dependencies",
  "version-string": "0.1.0",
  "dependencies": [
    "openssl",
    "zlib"
  ]
}
'@ | Set-Content -Path (Join-Path $manifestRoot 'vcpkg.json') -Encoding utf8

Write-Host 'Installing MSVC OpenSSL and zlib via vcpkg manifest mode...'
$previousLocation = Get-Location
try {
    Set-Location $manifestRoot
    & $vcpkg x-update-baseline --add-initial-baseline
    if ($LASTEXITCODE -ne 0) {
        throw "vcpkg baseline initialization failed with exit code $LASTEXITCODE."
    }
    $manifest = Get-Content -Path (Join-Path $manifestRoot 'vcpkg.json') -Raw | ConvertFrom-Json
    if (-not $manifest.'builtin-baseline') {
        throw 'vcpkg baseline initialization did not add builtin-baseline to the manifest.'
    }
    & $vcpkg install --triplet x64-windows --disable-metrics
    if ($LASTEXITCODE -ne 0) {
        throw "vcpkg dependency installation failed with exit code $LASTEXITCODE."
    }
} finally {
    Set-Location $previousLocation
}
$vcpkgInstallRoot = Join-Path $manifestRoot 'vcpkg_installed\x64-windows'

# Let CMake consume the manifest installation through vcpkg's toolchain.
$env:VCPKG_ROOT = $vcpkgRoot
$env:VCPKG_INSTALLED_DIR = Join-Path $manifestRoot 'vcpkg_installed'
if ($env:GITHUB_ENV) {
    "VCPKG_ROOT=$vcpkgRoot" | Out-File -FilePath $env:GITHUB_ENV -Encoding utf8 -Append
    "VCPKG_INSTALLED_DIR=$($env:VCPKG_INSTALLED_DIR)" | Out-File -FilePath $env:GITHUB_ENV -Encoding utf8 -Append
}

Write-Host 'Refreshing PATH for the current process...'
$machinePath = [Environment]::GetEnvironmentVariable('Path', 'Machine')
$userPath = [Environment]::GetEnvironmentVariable('Path', 'User')
$env:Path = ($machinePath + ';' + $userPath + ';C:\ProgramData\chocolatey\bin;C:\Program Files\CMake\bin;C:\Program Files\Ninja\bin')

foreach ($tool in @('curl', 'gpg')) {
    if (-not (Get-Command $tool -ErrorAction SilentlyContinue)) {
        throw "$tool is required but was not found after package installation."
    }
}
Write-Host "Using vcpkg installed tree: $env:VCPKG_INSTALLED_DIR"

Write-Host 'Installed toolchain versions:'
foreach ($tool in @('cmake', 'ninja', 'git', 'ccache')) {
    if (Get-Command $tool -ErrorAction SilentlyContinue) {
        & $tool --version
    } else {
        Write-Host "$tool --version failed: $tool not found on PATH."
    }
}
