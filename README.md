# chat
A simple C++ chat room using socket, supporting group chat and private chat.



## Build

```bash
$ ./make.sh
```

You will get the server and client in `build/`.



## Usage

Run the server：

```bash
$ ./server
```

Run the client with its name:

```bash
$ ./client client1
```

You may want to have several clients with different names.

```bash
$ ./client client2
$ ./client client3
```

**Group chat**

Just send your message, and all clients would receive it.

**Private chat**

Mention the client you want to chat with before entering your message( There must be a space between the name of client and your message), for example `@client2 hello!`. Only you and the client you mention would receive the message.



