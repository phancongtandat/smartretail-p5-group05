#include <climits>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

// T01: Data records and exact numeric parsing.
struct Product {
    std::string id, name, category, brand;
    long long price = 0;
    int ratingTenths = 0;
};
struct StockItem {
    std::string storeId, productId;
    int stock = 0;
};
struct Match {
    Product product;
    int sim = 0, stock = 0, adjustment = 0, finalScore = 0;
    std::string reasons;
};
struct Options {
    std::string folder, productId, storeId = "S001", brand, exportPath;
    long long maxPrice = LLONG_MAX;
    int k = 3;
    bool hasMaxPrice = false, filterBrand = false, exportRequested = false;
};

bool parseUnsigned(const std::string& text, long long maximum, long long& value) {
    if (text.empty()) return false;
    value = 0;
    for (char ch : text) {
        if (ch < '0' || ch > '9') return false;
        int digit = ch - '0';
        if (value > (maximum - digit) / 10) return false;
        value = value * 10 + digit;
    }
    return true;
}

bool toTenths(const std::string& text, int& value) {
    if (text.size() != 3 || text[1] != '.' || text[0] < '0' ||
        text[0] > '5' || text[2] < '0' || text[2] > '9') return false;
    value = (text[0] - '0') * 10 + text[2] - '0';
    return value <= 50;
}

std::vector<std::string> splitCsv(std::string line) {
    if (!line.empty() && line.back() == '\r') line.pop_back();
    std::vector<std::string> fields;
    std::string field;
    for (char ch : line) {
        if (ch == ',') {
            fields.push_back(field);
            field.clear();
        } else {
            field += ch;
        }
    }
    fields.push_back(field);
    return fields;
}

void warn(const std::string& path, int line, const std::string& reason) {
    std::cerr << "Warning " << path << ':' << line << ": " << reason << " -> skipped\n";
}

// T02: Read products, validate records, keep the first valid occurrence.
bool loadProducts(const std::string& path, std::vector<Product>& products) {
    std::ifstream input(path);
    if (!input) {
        std::cerr << "Error: cannot open " << path << '\n';
        return false;
    }
    std::string line;
    if (!std::getline(input, line) || splitCsv(line) !=
        std::vector<std::string>{"product_id", "name", "category", "brand", "price", "rating"}) {
        std::cerr << "Error: invalid products header in " << path << '\n';
        return false;
    }
    int lineNumber = 1;
    while (std::getline(input, line)) {
        ++lineNumber;
        std::vector<std::string> fields = splitCsv(line);
        if (fields.size() != 6) {
            warn(path, lineNumber, "expected 6 columns");
            continue;
        }
        Product product;
        if (fields[0].empty() || fields[1].empty() || fields[2].empty() || fields[3].empty() ||
            !parseUnsigned(fields[4], LLONG_MAX, product.price) ||
            !toTenths(fields[5], product.ratingTenths)) {
            warn(path, lineNumber, "invalid product fields, price or rating");
            continue;
        }
        bool duplicate = false;
        for (const Product& existing : products) {
            if (existing.id == fields[0]) duplicate = true;
        }
        if (duplicate) {
            warn(path, lineNumber, "duplicate product_id " + fields[0]);
            continue;
        }
        product.id = fields[0];
        product.name = fields[1];
        product.category = fields[2];
        product.brand = fields[3];
        products.push_back(product);
    }
    if (input.bad()) {
        std::cerr << "Error: failed while reading " << path << '\n';
        return false;
    }
    return true;
}

// T05: Selection sort by product ID, preparing the recursive binary search.
void sortProductsById(std::vector<Product>& products) {
    for (std::size_t i = 0; i < products.size(); ++i) {
        std::size_t best = i;
        for (std::size_t j = i + 1; j < products.size(); ++j) {
            if (products[j].id < products[best].id) best = j;
        }
        Product temporary = products[i];
        products[i] = products[best];
        products[best] = temporary;
    }
}

// T04: Recursive binary search. The input vector must be sorted by ID.
int findProduct(const std::vector<Product>& products, const std::string& id, int left, int right) {
    if (left > right) return -1;
    int middle = left + (right - left) / 2;
    if (products[middle].id == id) return middle;
    if (id < products[middle].id) return findProduct(products, id, left, middle - 1);
    return findProduct(products, id, middle + 1, right);
}

int stockOf(const std::vector<StockItem>& inventory, const std::string& store,
            const std::string& productId) {
    for (std::size_t i = 0; i < inventory.size(); ++i) {
        if (inventory[i].storeId == store && inventory[i].productId == productId)
            return static_cast<int>(i);
    }
    return -1;
}

// T03: Inventory keys are (store_id, product_id), not product_id alone.
bool loadInventory(const std::string& path, const std::vector<Product>& products,
                   std::vector<StockItem>& inventory) {
    std::ifstream input(path);
    if (!input) {
        std::cerr << "Error: cannot open " << path << '\n';
        return false;
    }
    std::string line;
    if (!std::getline(input, line) || splitCsv(line) !=
        std::vector<std::string>{"store_id", "product_id", "stock"}) {
        std::cerr << "Error: invalid inventory header in " << path << '\n';
        return false;
    }
    int lineNumber = 1;
    while (std::getline(input, line)) {
        ++lineNumber;
        std::vector<std::string> fields = splitCsv(line);
        long long stock = 0;
        if (fields.size() != 3) {
            warn(path, lineNumber, "expected 3 columns");
            continue;
        }
        if (fields[0].empty() || fields[1].empty() || !parseUnsigned(fields[2], INT_MAX, stock)) {
            warn(path, lineNumber, "invalid inventory fields or stock");
            continue;
        }
        if (findProduct(products, fields[1], 0, static_cast<int>(products.size()) - 1) < 0) {
            warn(path, lineNumber, "unknown product_id " + fields[1]);
            continue;
        }
        if (stockOf(inventory, fields[0], fields[1]) >= 0) {
            warn(path, lineNumber, "duplicate inventory key " + fields[0] + "/" + fields[1]);
            continue;
        }
        inventory.push_back({fields[0], fields[1], static_cast<int>(stock)});
    }
    if (input.bad()) {
        std::cerr << "Error: failed while reading " << path << '\n';
        return false;
    }
    return true;
}

// T07: All four checks are independent. Integer thresholds avoid rounding and overflow.
int similarity(const Product& q, const Product& c, std::string& reasons) {
    int sim = 0;
    reasons.clear();
    if (c.category == q.category) {
        sim += 4;
        reasons = "category +4";
    }
    if (c.brand == q.brand) {
        sim += 3;
        if (!reasons.empty()) reasons += ", ";
        reasons += "brand +3";
    }
    const long long priceGap = std::llabs(c.price - q.price);
    if (q.price > 0 && priceGap <= (q.price - 1) / 5) {
        sim += 2;
        if (!reasons.empty()) reasons += ", ";
        reasons += "close price +2";
    }
    const int ratingGap = std::abs(c.ratingTenths - q.ratingTenths);
    if (ratingGap < 3) {
        sim += 1;
        if (!reasons.empty()) reasons += ", ";
        reasons += "close rating +1";
    }
    return sim;
}

// T08: Stock only adjusts ranking.
int availabilityAdjustment(int stock) {
    if (stock == 0) return -5;
    if (stock <= 2) return 0;
    return 1;
}

std::string availableElsewhere(const std::vector<StockItem>& inventory,
                               const std::string& store, const std::string& productId) {
    std::string result;
    for (const StockItem& item : inventory) {
        if (item.productId == productId && item.storeId != store && item.stock >= 1) {
            if (!result.empty()) result += ", ";
            result += item.storeId + " (stock " + std::to_string(item.stock) + ")";
        }
    }
    if (result.empty()) return "no other store";
    return result;
}

// T06: Strict lexicographic comparator and handwritten selection sort.
bool comesBefore(const Match& a, const Match& b) {
    if (a.finalScore != b.finalScore) return a.finalScore > b.finalScore;
    if (a.sim != b.sim) return a.sim > b.sim;
    if (a.product.ratingTenths != b.product.ratingTenths)
        return a.product.ratingTenths > b.product.ratingTenths;
    return a.product.id < b.product.id;
}

void sortMatches(std::vector<Match>& matches) {
    for (std::size_t i = 0; i < matches.size(); ++i) {
        std::size_t best = i;
        for (std::size_t j = i + 1; j < matches.size(); ++j) {
            if (comesBefore(matches[j], matches[best])) best = j;
        }
        Match temporary = matches[i];
        matches[i] = matches[best];
        matches[best] = temporary;
    }
}

// T09: Filter before scoring, exclude query, then rank for the requested store.
std::vector<Match> rankMatches(const Product& query, const std::vector<Product>& products,
                              const std::vector<StockItem>& inventory, const std::string& store,
                              const Options& options, int& excluded) {
    std::vector<Match> matches;
    excluded = 0;
    for (const Product& product : products) {
        if (product.id == query.id) continue;
        if (options.hasMaxPrice && product.price > options.maxPrice) {
            ++excluded;
            continue;
        }
        if (options.filterBrand && product.brand != options.brand) continue;
        Match match;
        match.product = product;
        match.sim = similarity(query, product, match.reasons);
        int position = stockOf(inventory, store, product.id);
        if (position >= 0) match.stock = inventory[position].stock;
        match.adjustment = availabilityAdjustment(match.stock);
        match.finalScore = match.sim + match.adjustment;
        matches.push_back(match);
    }
    sortMatches(matches);
    return matches;
}

std::string ratingText(int tenths) {
    return std::to_string(tenths / 10) + "." + std::to_string(tenths % 10);
}

// T11: Bound Top-K by the available candidate count.
void printTopK(const std::vector<Match>& matches, const std::vector<StockItem>& inventory,
               const Options& options) {
    int count = options.k;
    if (static_cast<int>(matches.size()) < count) count = static_cast<int>(matches.size());
    std::cout << "\nTOP " << count << " SIMILAR PRODUCTS\n";
    for (int i = 0; i < count; ++i) {
        const Match& match = matches[i];
        std::cout << i + 1 << ". " << match.product.id << ' ' << match.product.name
                  << " | " << match.finalScore << " pts | ";
        if (match.stock == 0) std::cout << "OUT OF STOCK - pre-order";
        else if (match.stock <= 2) std::cout << "only " << match.stock << " left";
        else std::cout << match.stock << " in stock";
        std::cout << "\n   reasons: " << (match.reasons.empty() ? "none" : match.reasons) << '\n';
        if (match.stock == 0)
            std::cout << "   in stock at: "
                      << availableElsewhere(inventory, options.storeId, match.product.id) << '\n';
    }
    int highestSimilarity = -1;
    for (const Match& match : matches) {
        if (match.sim > highestSimilarity) highestSimilarity = match.sim;
    }
    // Attribute a demotion to stock only when a lower-similarity product precedes it.
    for (std::size_t i = 0; i < matches.size(); ++i) {
        if (matches[i].stock != 0 || matches[i].sim != highestSimilarity) continue;
        bool demoted = false;
        for (std::size_t j = 0; j < i; ++j) {
            if (matches[j].sim < matches[i].sim) demoted = true;
        }
        if (demoted)
            std::cout << "Note: " << matches[i].product.id << " is the most similar ("
                      << highestSimilarity << "/10) but drops to rank " << i + 1
                      << " because it is out of stock.\n";
    }
}

// T12: Use the same filters, but each store's own stock.
void printBestSubstitutes(const Product& query, const std::vector<Product>& products,
                          const std::vector<StockItem>& inventory, const Options& options) {
    std::vector<std::string> stores;
    for (const StockItem& item : inventory) {
        bool known = false;
        for (const std::string& store : stores) {
            if (store == item.storeId) known = true;
        }
        if (!known) stores.push_back(item.storeId);
    }
    // Canonical output order independent of CSV row order.
    for (std::size_t i = 0; i < stores.size(); ++i) {
        for (std::size_t j = i + 1; j < stores.size(); ++j) {
            if (stores[j] < stores[i]) {
                std::string temporary = stores[i]; stores[i] = stores[j]; stores[j] = temporary;
            }
        }
    }
    std::cout << "\nBEST SUBSTITUTE BY STORE\n";
    if (stores.empty()) std::cout << "No stores in valid inventory.\n";
    for (const std::string& store : stores) {
        int excluded = 0;
        std::vector<Match> matches = rankMatches(query, products, inventory, store, options, excluded);
        std::cout << store << " | ";
        if (matches.empty()) std::cout << "no matching candidates";
        else std::cout << matches[0].product.id << ' ' << matches[0].product.name << " | "
                       << matches[0].finalScore << " pts | stock " << matches[0].stock;
        if (store == options.storeId) std::cout << " <- current store";
        std::cout << '\n';
    }
}

// T10: Complete report.
void printReport(const Product& query, const std::vector<Product>& products,
                 const std::vector<StockItem>& inventory, const std::vector<Match>& matches,
                 const Options& options, int excluded) {
    std::cout << "QUERY: " << query.id << ' ' << query.name << " (" << query.category << ", "
              << query.brand << ", " << query.price << ", " << ratingText(query.ratingTenths)
              << ") @" << options.storeId << "\n\n";
    std::cout << "ID | Product | Sim | Stock | Adj | Final\n";
    if (matches.empty()) std::cout << "No matching candidates.\n";
    for (const Match& match : matches) {
        std::cout << match.product.id << " | " << match.product.name << " | " << match.sim
                  << " | " << match.stock << " | " << std::showpos << match.adjustment
                  << std::noshowpos << " | " << match.finalScore;
        if (match.stock == 0) std::cout << " << OUT OF STOCK";
        std::cout << '\n';
    }
    printTopK(matches, inventory, options);
    printBestSubstitutes(query, products, inventory, options);
    if (options.hasMaxPrice)
        std::cout << "Excluded (above max price " << options.maxPrice << "): " << excluded << " products.\n";
}

// T14: Optional CSV export, with observable open/write errors.
bool exportTopK(const std::vector<Match>& matches, const Options& options) {
    std::ofstream output(options.exportPath);
    if (!output) {
        std::cerr << "Error: cannot open export file " << options.exportPath << '\n';
        return false;
    }
    output << "rank,store_id,product_id,sim,stock,final\n";
    int count = options.k;
    if (static_cast<int>(matches.size()) < count) count = static_cast<int>(matches.size());
    for (int i = 0; i < count; ++i) {
        const Match& match = matches[i];
        output << i + 1 << ',' << options.storeId << ',' << match.product.id << ','
               << match.sim << ',' << match.stock << ',' << match.finalScore << '\n';
    }
    output.close();
    if (!output) {
        std::cerr << "Error: failed writing export file " << options.exportPath << '\n';
        return false;
    }
    return true;
}

// T13: Positional CLI arguments plus independent named options.
bool parseArguments(int argc, char* argv[], Options& options) {
    if (argc < 3) {
        std::cerr << "Usage: p5 <data_folder> <product_id> [store_id] [K] [max_price]"
                  << " [--brand <brand>] [--export <file.csv>]\n";
        return false;
    }
    options.folder = argv[1]; options.productId = argv[2];
    std::vector<std::string> positional;
    for (int i = 3; i < argc; ++i) {
        std::string argument = argv[i];
        if (argument == "--brand" || argument == "--export") {
            if (i + 1 >= argc || std::string(argv[i + 1]).empty() ||
                std::string(argv[i + 1]).find("--") == 0) {
                std::cerr << "Error: missing value for " << argument << '\n'; return false;
            }
            bool duplicate = argument == "--brand" ? options.filterBrand : options.exportRequested;
            if (duplicate) {
                std::cerr << "Error: repeated option " << argument << '\n'; return false;
            }
            if (argument == "--brand") {
                options.filterBrand = true; options.brand = argv[++i];
            } else {
                options.exportRequested = true; options.exportPath = argv[++i];
            }
        } else if (argument.find("--") == 0) {
            std::cerr << "Error: unknown option " << argument << '\n'; return false;
        } else {
            positional.push_back(argument);
        }
    }
    if (positional.size() > 3) {
        std::cerr << "Error: too many positional arguments\n"; return false;
    }
    if (!positional.empty()) options.storeId = positional[0];
    long long number = 0;
    if (positional.size() >= 2) {
        if (!parseUnsigned(positional[1], INT_MAX, number) || number == 0) {
            std::cerr << "Error: K must be a positive integer within int range\n"; return false;
        }
        options.k = static_cast<int>(number);
    }
    if (positional.size() == 3) {
        if (!parseUnsigned(positional[2], LLONG_MAX, number) || number == 0) {
            std::cerr << "Error: max_price must be a positive integer within long long range\n";
            return false;
        }
        options.hasMaxPrice = true; options.maxPrice = number;
    }
    return true;
}

int main(int argc, char* argv[]) {
    Options options;
    if (!parseArguments(argc, argv, options)) return 1;
    std::vector<Product> products;
    std::vector<StockItem> inventory;
    if (!loadProducts(options.folder + "/products.csv", products)) return 1;
    sortProductsById(products);
    if (!loadInventory(options.folder + "/inventory.csv", products, inventory)) return 1;
    int position = findProduct(products, options.productId, 0, static_cast<int>(products.size()) - 1);
    if (position < 0) {
        std::cerr << "Error: query product not found: " << options.productId << '\n'; return 1;
    }
    int excluded = 0;
    std::vector<Match> matches = rankMatches(products[position], products, inventory,
                                            options.storeId, options, excluded);
    printReport(products[position], products, inventory, matches, options, excluded);
    if (options.exportRequested && !exportTopK(matches, options)) return 1;
    return 0;
}
