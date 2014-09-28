#!/boot/home/config/bin/rebol  

REBOL [Title: "REBOL Comms Demo Script"]

server: open [ 
        scheme: 'tcp 
        host: "127.0.0.1" 
        port-id: 9999 
    ]


test: "NNEWS"
append test #{00}
append test "/boot/home/newstest.txt"
append test #{00}
append test "alt.alt.alt"
append test #{00}
append test 1234
append test #{00}
append test "^J"


insert server test

quit