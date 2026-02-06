import random

patterns = [
    (0, 0, 1, 0),
    (0, 1, 1, 0),
    (1, 0, 1, 0),
    (1, 1, 1, 1)
]

# Initial weights
w = [0.5, 0.3, 0.3]   # w1, w2, wb
threshold = 0.4

epoch = 1

c = (float)(input("Enter learning rate (0 - 1 floating value): "))

while True:
    print(f"\nEpoch {epoch}")
    print("x1 x2 b | t | weights(w1 w2 wb) | net | z | Δw1 Δw2 Δwb")
    print("-" * 70)

    errors = 0

    for x1, x2, b, t in patterns:
        net = x1*w[0] + x2*w[1] + b*w[2]

        # Activation
        z = 1 if net > threshold else 0

        error = t - z

        # Weight updates
        dw1 = c * error * x1
        dw2 = c * error * x2
        dwb = c * error * b
        
        # Print row
        print(
            f"{x1:>1}  {x2:>1}  {b:>1} | {t:>1} | "
            f"{w[0]:.2f} {w[1]:.2f} {w[2]:.2f} | "
            f"{net:.2f} | {z} | "
            f"{dw1:.2f} {dw2:.2f} {dwb:.2f}"
        )

        w[0] += dw1
        w[1] += dw2
        w[2] += dwb

        if dw1 != 0 or dw2 != 0 or dwb != 0:
            errors += 1

    print(f"\nNumber of errors in epoch {epoch}: {errors}")

    # Stop when errors become zero
    if errors == 0:
        print(f"Optimal weights found: w1: {w[0]:.2f}, w2: {w[1]:.2f}, w3: {w[2]:.2f}")
        print("Made by Crabbyfeet")
        break

    epoch += 1
