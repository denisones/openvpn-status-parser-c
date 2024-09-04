# openvpn-status-parser-c

A simple parser for parsing OpenVPN status files

## Usage

```
./openvpn-status-parser /etc/openvpn/server-tcp-status.log

```

## Example output
```json
{
    "title": "OpenVPN 2.4.4 x86_64-redhat-linux-gnu",
    "time": [
        "Sat Aug 24 23:41:43 2024",
        "1724532103"
    ],
    "clientList": [
        {
            "commonName": "client01",
            "realAddress": "100.0.0.0:100",
            "virtualAddress": "10.0.0.0",
            "virtualIPv6Address": "",
            "bytesReceived": 9093744,
            "bytesSent": 277053815,
            "connectedSince": "Sat Aug 24 21:31:32 2024",
            "connectedSinceTime_t": 1724524292,
            "username": "UNDEF",
            "clientId": 223,
            "peerId": 0
        }
    ],
    "routingTable": [
        {
            "virtualAddress": "10.0.0.0",
            "commonName": "client01",
            "realAddress": "100.0.0.0:100",
            "lastRef": "Sat Aug 24 23:41:42 2024",
            "lastRefTime_t": 1724532102
        }
    ],
    "header": {
        "clientList": [
            "Common Name",
            "Real Address",
            "Virtual Address",
            "Virtual IPv6 Address",
            "Bytes Received",
            "Bytes Sent",
            "Connected Since",
            "Connected Since (time_t)",
            "Username",
            "Client ID",
            "Peer ID"
        ],
        "routingTable": [
            "Virtual Address",
            "Common Name",
            "Real Address",
            "Last Ref",
            "Last Ref (time_t)"
        ]
    }
}
```