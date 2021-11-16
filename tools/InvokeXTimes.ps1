Set-StrictMode -Version 3.0

$Script:XTimes = 2
if ($args.Count -eq 1) {
    $Script:XTimes = $args[0]
}

$Script:ExeToInvoke = '..\..\TinyOrmPlayground-builds-qmake\build-TinyOrmPlayground-Desktop_Qt_5_15_2_MSVC2019_64bit-Debug\debug\TinyOrmPlayground.exe'
if ($args.Count -eq 2)
{
    $Script:ExeToInvoke = $args[1]
}

for ($i = 0; $i -lt $Script:XTimes; $i++)
{
    Write-Host "Running $($i + 1). time..." -ForegroundColor DarkGreen

    & $Script:ExeToInvoke > InvokeXTimes-out.log 2> InvokeXTimes-err.log

    if ($LASTEXITCODE -ne 0)
    {
        throw "Last Exit code was not 0, it was: $LASTEXITCODE"
    }
}
