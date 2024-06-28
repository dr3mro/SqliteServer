import requests
import zlib

url = "http://127.0.0.1:8080/api_v1/read_patient_basic_information/100003"

try:
    # Make the request with 'Accept-Encoding: gzip' header
    headers = {'Accept-Encoding': 'gzip'}
    with requests.get(url, headers=headers) as response:
        print(f"Response from {url}: Status Code {response.status_code}")

        # Check if content is gzip encoded
        if response.headers.get('Content-Encoding') == 'gzip':
            # Decode gzip content
            try:
                content = zlib.decompress(response.content, zlib.MAX_WBITS | 16)
                print(f"Decoded Content: {content.decode('utf-8')}")
            except zlib.error as e:
                print(f"Error decoding gzip content: {e}")
        else:
            print(f"Response Content (not gzip encoded): {response.text}")

except requests.RequestException as e:
    print(f"Request to {url} failed: {e}")
