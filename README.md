# Checkers
Client - server project

![](Games.png)

## Before you run

You have to install Tkinter
```
sudo apt-get install python3-tk
```

You have to install Python libraries
```
make install_client_lib
```

Compile C++ server
```
make compile_server
```

## Run

Start the server
```
make run_server port=1234
```

Next start two instance of the client
```
make run_client
```
