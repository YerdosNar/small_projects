import os

name = os.getenv("USER", "Yerdos")
print(f"Hello {name} from Python")

cols = os.getenv("COLUMNS")
print(f"Columns: {cols}")

id = os.getenv("ID", "unknown")
print(f"OS ID: {id}")
