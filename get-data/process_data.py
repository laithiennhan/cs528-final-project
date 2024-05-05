import re

TAG = "final-project"
ansi_escape = re.compile(r"\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])")


def process_data(filename, movement):
    with open(filename, "r") as f:
        index = 0
        f_out = open("datasets/{}_{:02d}.txt".format(movement, index), "w")
        for line in f:
            tokens = ansi_escape.sub("", line).strip().split()
            if TAG in tokens:
                tokens = tokens[tokens.index(TAG) + 1 :]
                if tokens[0] == "end":
                    f_out.close()
                    index += 1
                    f_out = open("datasets/{}_{:02d}.txt".format(movement, index), "w")
                else:
                    f_out.write(",".join(tokens) + "\n")

        f_out.close()


if __name__ == "__main__":
    process_data("up.txt", "up")
    process_data("down.txt", "down")
    process_data("right.txt", "right")
    process_data("left.txt", "left")
