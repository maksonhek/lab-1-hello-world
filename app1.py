import os

import mytools

DATA_FILE = os.environ.get("EXPENSES_FILE", "data/expenses.json")


def menu():
    print("Калькулятор расходов")
    print("Меню")
    print("1 – добавить расход ")
    print("2 – показать все ")
    print("3 – сумма по категории ")
    print("0 – выход")


def add_expenses(expenses):
    amount = mytools.ask_number("Сумма:")
    category = mytools.ask_text("Категория:")
    expense = {"amount": amount, "category": category, "date": mytools.today()}
    expenses.append(expense)
    mytools.save_json(DATA_FILE, expenses)
    print(f"Добавлено: {category} — {mytools.format_money(amount)}")


def show_all(expenses):
    if not expenses:
        print("список пуст")
        return
    print("Расходы")
    for i, exp in enumerate(expenses, start=1):
        print(
            f"  {i}. {exp['date']}  {exp['category']} — {mytools.format_money(exp['amount'])}"
        )


def sum_by_category(expenses):
    if not expenses:
        print("Список пуст")
        return
    category = mytools.ask_text("Категория:").lower()
    total = 0
    count = 0
    for exp in expenses:
        if exp["category"].lower() == category:
            total += exp["amount"]
            count += 1
    if count == 0:
        print(f"По категории {category} ничего не найдено")
    else:
        print(f"Сумма по категориями {category} : {mytools.format_money(total)}")


def main():
    expenses = mytools.load_json(DATA_FILE)
    print(f"Данные:{DATA_FILE}(загружено записей: {len(expenses)})")
    while True:
        menu()
        choice = mytools.ask_text("Выбор:")
        if choice == "1":
            add_expenses(expenses)
        elif choice == "2":
            show_all(expenses)
        elif choice == "3":
            sum_by_category(expenses)
        elif choice == "0":
            break
        else:
            print(f"Неизвестная команда{choice}. Попробуйте 0-3")


main()
