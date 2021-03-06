compile_server:
	g++ -pthread functions.cpp server.cpp -Wall -o server

install_client_lib:
	python3 -m pip install -r requirements.txt

run_server:
	./server $(port)

run_client:
	python3 client.py
