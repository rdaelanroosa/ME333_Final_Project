def ping():
    print("ping")

dict = {'a':ping}

def pong():
    print("womp!")

dict.get(input('>>>'), pong)()