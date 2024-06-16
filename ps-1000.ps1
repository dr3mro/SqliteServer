# Function to generate a random string of a specified length
function Generate-RandomString {
    param (
        [int]$length = 8
    )
    $chars = [char[]]"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    -join (1..$length | ForEach-Object { $chars[(Get-Random -Minimum 0 -Maximum $chars.Length)] })
}

# URL and headers
$url = "http://localhost:18080/post"
$headers = @{
    "Accept" = "application/json"
    "Content-type" = "application/json"
}

# Loop to create 1000 POST requests
for ($i = 1; $i -le 1000; $i++) {
    $id = $i
    $value = Generate-RandomString
    $body = @{
        id = $id
        value = $value
    } | ConvertTo-Json

    try {
        $response = Invoke-RestMethod -Uri $url -Method Post -Headers $headers -Body $body
        Write-Output "POST ID: $id, Response: $($response | ConvertTo-Json -Depth 10)"
    } catch {
        Write-Output "POST ID: $id, Error: $_"
    }
}
