import json
import os
from datetime import date


def ask_number(prompt: float):
    while True:
        x = input(prompt).strip()
        try:
            return float(x)
        except ValueError:
            print("Введите число")


def ask_text(prompt: str):
    while True:
        text = input(prompt).strip()
        if text:
            return text
        print("Строка не должна быть пустой")


def format_money(x: float):
    return f"{x:,.2f}".replace(",", " ") + "₽"


def today():
    return date.today().isoformat()


def load_json(path: str):
    if not os.path.exists(path):
        return []
    try:
        with open(path, "r") as f:
            return json.load(f)
    except json.JSONDecodeError:
        return []


def save_json(path: str, data):
    folder = os.path.dirname(path)
    if folder:
        os.makedirs(folder, exist_ok=True)
    with open(path, "w") as f:
        json.dump(data, f, ensure_ascii=False, indent=2)
