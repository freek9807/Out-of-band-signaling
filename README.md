# Out of band signaling

This is my Operating System project (I take the course in UNIPI).

# Description

The supervisor launch k servers and wait the information of n clients started independently . <br />
The clients' informations are : id ( it'll send directly ) and the secret message ( it'll calculated from the server) . <br />
The secret is the time passed between two client messages and, the purpose of the system is to estimate all of them . <br />


## Usage

```Bash
make && make test 
```

## License
[MIT](https://choosealicense.com/licenses/mit/)
