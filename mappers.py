from itertools import islice

#ZAMIANA STRINGA POSTACI 1234 NA LISTE
def str_to_list(test_str):
    return [int(i) for i in test_str]

#ZAMIANA 1D LIST NA 2D
def convert_1d_to_2d(lst, len_lst):
    it = iter(lst)
    return [list(islice(it, i)) for i in len_lst]

#return 2 dimensional list
def convert_board(rec_board):
    len_lst = [8, 8, 8, 8, 8, 8, 8, 8]
    res = str_to_list(rec_board)
    res2d = convert_1d_to_2d(res, len_lst)
    for i in range(8):
        print(res2d[i])
    return res2d

def move_to_string(start, end):
    move = start + end
    #lista string
    data = [str(x) for x in move] 
    send_str = ''.join(data)
    return send_str