from fuzzywuzzy import fuzz
import csv


def csv_to_dict(csv_file):
    data = {}
    with open(csv_file, newline="", encoding="utf-8") as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            symbol = row["Symbol"]
            del row["Symbol"]
            data[symbol] = list(row.values())
    return data


def rank_strings(strings, target_string):
    lower_target_string = target_string.lower()
    ranked_strings = sorted(
        strings,
        key=lambda x: fuzz.partial_ratio(x.lower(), lower_target_string),
        reverse=True,
    )
    return ranked_strings


def top_4_stocks(target):
    data = csv_to_dict("ind_nifty100list.csv")
    symbols = list(data.keys())
    company_names = [details[0] for details in data.values()]
    dict1 = {}
    for i in range(100):
        dict1[symbols[i]] = symbols[i] + " " + company_names[i]

    ranked_symbols = rank_strings(list(dict1.values()), target)
    ranked_dict = {k: v for v, k in enumerate(ranked_symbols, start=1)}
    ranked_dict = {value: key for key, value in ranked_dict.items()}
    dict1 = {value: key for key, value in dict1.items()}
    ans = []
    ans.append([dict1[ranked_dict[1]], data[dict1[ranked_dict[1]]][0]])
    ans.append([dict1[ranked_dict[2]], data[dict1[ranked_dict[2]]][0]])
    ans.append([dict1[ranked_dict[3]], data[dict1[ranked_dict[3]]][0]])
    ans.append([dict1[ranked_dict[4]], data[dict1[ranked_dict[4]]][0]])

    return ans
