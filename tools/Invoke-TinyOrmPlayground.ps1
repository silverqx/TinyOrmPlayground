Param(
    [Parameter(Position = 0, Mandatory = $false,
               HelpMessage = 'Specifies how many times an executable should be invoked.')]
    [ValidateNotNullOrEmpty()]
    [int] $Count = 2,

    [Parameter(Position = 1, Mandatory = $false,
               HelpMessage = 'Specifies an executable to invoke.')]
    [ValidateNotNullOrEmpty()]
    [string] $Executable = '..\..\TinyOrmPlayground-builds-qmake\build-TinyOrmPlayground-Desktop_Qt_6_5_0_MSVC2022_64bit-Debug\debug\TinyOrmPlayground.exe'
)

Set-StrictMode -Version 3.0

$Script:ConnectionsInThreads = $null

function Initialize-ConnectionsInThreads {
    $regex = Get-Content -Path .\Invoke-TinyOrmPlayground-err.log -Tail 20
        | Select-String -Pattern 'Threads mode +: (?<threadsMode>[\w\-]+)'

    if ($null -eq $regex) {
        throw 'Can not detect threads mode, no regex matches.'
    }

    $threadsMode = $regex.Matches[0].Groups['threadsMode'].Value

    if ($threadsMode -ceq 'single-thread') {
        $Script:ConnectionsInThreads = $false
    }
    elseif ($threadsMode -ceq 'multi-thread') {
        $Script:ConnectionsInThreads = $true
    }
    else {
        throw 'Can not detect threads mode.'
    }
}

function Get-LastElapsed {
    [OutputType([int])]
    Param(
        [Parameter(Position = 0, Mandatory = $true, HelpMessage = "Specifies the text to find on each line. The pattern value is treated as a regular expression.")]
        [ValidateNotNullOrEmpty()]
        [string]
        $Pattern
    )

    $regex = Select-String -Pattern $Pattern -Path .\Invoke-TinyOrmPlayground-err.log

    if ($null -eq $regex) {
        Write-Host
        throw "Pattern 'lastElapsed' not found, any regex matches."
    }

    $lastElapsed = $regex.Matches.Groups
        | Where-Object Name -eq lastElapsed
        | Select-Object -ExpandProperty Value -Last 1

    return $lastElapsed
}

function Get-AverageElapsed {
    [OutputType([int])]
    Param(
        [Parameter(Position = 0, Mandatory = $true, HelpMessage = "Specifies the text to find on each line. The pattern value is treated as a regular expression.")]
        [ValidateNotNullOrEmpty()]
        [string]
        $Pattern
    )

    $regex = Select-String -Pattern $Pattern -Path .\Invoke-TinyOrmPlayground-err.log

    if ($null -eq $regex) {
        Write-Host
        throw "Pattern 'averageElapsed' not found, any regex matches."
    }

    [int] $averageElapsed = $regex.Matches.Groups
        | Where-Object Name -eq averageElapsed
        | Select-Object -ExpandProperty Value
        | Measure-Object -Average
        | Select-Object -ExpandProperty Average

    return $averageElapsed
}

# Empty logs content
if (Test-Path -Path Invoke-TinyOrmPlayground-out.log -PathType Leaf) {
    Clear-Content Invoke-TinyOrmPlayground-out.log
}
if (Test-Path -Path Invoke-TinyOrmPlayground-err.log -PathType Leaf) {
    Clear-Content Invoke-TinyOrmPlayground-err.log
}

for ($i = 0; $i -lt $Count; $i++) {
    Write-Host "Running $($i + 1). time..." -NoNewline

    & $Executable >> Invoke-TinyOrmPlayground-out.log 2>> Invoke-TinyOrmPlayground-err.log

    if ($LASTEXITCODE -ne 0) {
        Write-Host 'failed' -ForegroundColor Red
        Write-Host
        throw "Last Exit code was not 0, it was: $LASTEXITCODE"
    }

    Write-Host 'done -' -NoNewline

    if ($null -eq $Script:ConnectionsInThreads) {
        Initialize-ConnectionsInThreads
    }

    $functionsElapsed = -1
    $queriesElapsed = -1

    # Single-thread mode
    if (-not $Script:ConnectionsInThreads) {
        # Functions execution time
        $functionsElapsed = Get-LastElapsed `
            -Pattern 'Functions execution time +: (?<lastElapsed>\d+)ms'

        Write-Host " Functions " -NoNewline
        Write-Host "${functionsElapsed}ms," -ForegroundColor DarkGreen -NoNewline

        # Queries execution time
        $queriesElapsed = Get-LastElapsed -Pattern 'Queries execution time +: (?<lastElapsed>\d+)ms'

        Write-Host " Queries " -NoNewline
        Write-Host "${queriesElapsed}ms," -ForegroundColor DarkGreen -NoNewline
    }

    # Total execution time
    $totalElapsed = Get-LastElapsed `
        -Pattern 'Total TestOrm instance execution time : (?<lastElapsed>\d+)ms'

    Write-Host " Total " -NoNewline
    Write-Host "${totalElapsed}ms" -ForegroundColor DarkGreen
}

Write-Host

# Single-thread mode
if (-not $Script:ConnectionsInThreads) {
    # Average Functions execution time
    $functionsAverage = Get-AverageElapsed `
        -Pattern 'Functions execution time +: (?<averageElapsed>\d+)ms'

    Write-Host "Average Functions execution time         :" -NoNewline
    Write-Host " $($functionsAverage)ms" -ForegroundColor DarkGreen

    # Average Queries execution time, only Application Summary has {2,}
    $queriesAverage = Get-AverageElapsed `
        -Pattern 'Queries execution time {2,}: (?<averageElapsed>\d+)ms'

    Write-Host "Average Queries execution time           :" -NoNewline
    Write-Host " $($queriesAverage)ms" -ForegroundColor DarkGreen
}

# Average Total execution time
$totalAverage = Get-AverageElapsed `
    -Pattern 'Total TestOrm instance execution time : (?<averageElapsed>\d+)ms'

Write-Host "Average TestOrm instances execution time :" -NoNewline
Write-Host " $($totalAverage)ms" -ForegroundColor DarkGreen

# Thread mode
Write-Host
if ($Script:ConnectionsInThreads) {
    Write-Host 'Multi-thread' -ForegroundColor DarkCyan -NoNewline
}
else {
    Write-Host 'Single-thread' -ForegroundColor DarkCyan -NoNewline
}
Write-Host ' mode'
Write-Host
