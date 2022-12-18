## GRPC CLI
```
# Run some method on kv.KVStore with some JSON input
grpc_cli call localhost:50051 kv.KVStore/Set --json_input '{"key": "lol", "value": "1"}'
grpc_cli call localhost:50051 kv.KVStore/Incr --json_input '{"key": "lol", "incr": "1"}' 
grpc_cli call localhost:50051 kv.KVStore/Get --json_input '{"key": "lol"}'
```

## Wired Tiger CLI
```
# Dump the table config and the data
wt -h data dump -j 'table:tyler' | jq 

# Read a particular key from a table
wt -h data read 'table:tyler' key
```
