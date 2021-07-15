var fs = require('fs');
var WebSocket = require('ws');
var server = new WebSocket.Server({
    port: 8080
});

server.on('connection', function(socket) {

    socket.on('message', function(msg) {
        console.log(msg);
        fs.writeFile('input.txt', msg, (error) => {
            if (error) throw err;
        });

    });

    socket.on('close', function() {
        console.log('socket closed');

    });
