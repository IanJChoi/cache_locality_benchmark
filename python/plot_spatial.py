import csv
from pathlib import Path
import matplotlib.pyplot as plt


def main():
    root = Path(__file__).resolve().parents[1]
    csv_path = root / "output" / "spatial.csv"
    out_path = root / "output" / "spatial.png"

    sizes = []
    seq = []
    chn = []

    with csv_path.open() as f:
        reader = csv.DictReader(f)
        for row in reader:
            if row["method"] == "sequential":
                sizes.append(int(row["bytes"]))
                seq.append(float(row["time_per_elem"]))
            elif row["method"] == "chunked":
                chn.append(float(row["time_per_elem"]))

    plt.figure(figsize=(8, 4.8))
    plt.plot(sizes, seq, marker="o", label="sequential")
    plt.plot(sizes, chn, marker="o", label="chunked")
    plt.xscale("log", base=2)
    plt.xlabel("Array size (bytes)")
    plt.ylabel("Time per element (s)")
    plt.title("Spatial locality test")
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)
    plt.xticks(sizes, [f"{b//1024}KB" if b < 1024 * 1024 else f"{b//1024//1024}MB" for b in sizes])
    plt.legend()
    plt.tight_layout()
    plt.savefig(out_path, dpi=160)


if __name__ == "__main__":
    main()
