#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>


class Expense {
    public:
    int ID;
    std::string description;
    long long amount;
    std::string category;
    std::chrono::system_clock::time_point timestamp;

    Expense(int id, const std::string &desc, long long amt, const std::string &cat ) : ID(id), description(desc), amount (amt),category(cat) {timestamp = std::chrono::system_clock::now();}
    
    std::string getTimeString() const {
        std::time_t time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};


class ExpenseManager {
    private:
    std::vector<Expense> expenses;
    int nextID = 1;

    public:
    void addExpense(const std::string& description, long long amount, const std::string& category);
    void listExpense() const;
    void updateExpense(int id);
    void deleteExpense(int id);
    void viewSummary();
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename) ;
};

void ExpenseManager::addExpense(const std::string& description, long long amount, const std::string& category) {
    Expense e(nextID, description, amount, category);
    expenses.push_back(e);
    nextID++;    
    std::cout << "expense added. \n";
}

void ExpenseManager::updateExpense(int id) {
    listExpense();   
    std::cout << "update <id> expense <description><amount><category>" << std::endl;
    bool found = false;
    
    for (auto& e : expenses) {
        if (id == e.ID) {
            found = true;
            std::string val;
            std::cout << "type 'description' or 'amount' or 'category' to update accordingly." << std::endl;
            std::cin >> val;
            
            if (val == "description") {
                std::cout << "updated description: " << std::endl;
                std::cin.ignore();
                std::getline(std::cin, e.description);
            }
            else if (val == "amount") {
                std::cout << "updated amount: " << std::endl;
                std::cin >> e.amount;
            
            }
            else if (val == "category") {
                std::cout << "updated category: " << std::endl;
                std::cin >> e.category;
            }
            break;
        } 
    }

    if (!found) {
        std::cout << "ID not found.\n";
    }
}

void ExpenseManager::listExpense() const {
    if(expenses.empty()) {
        std::cout << "no expenses added yet. \n";
        return;
    }

    std::cout << "---------------------------------------------------------------------\n";
    for (const auto& e : expenses) {
        std::cout << std::left << std::setw(4) << e.ID
          << std::setw(20) << e.description
          << "$" << std::setw(10) << e.amount
          << std::setw(15) << e.category
          << e.getTimeString() << "\n";
    }
}    

void ExpenseManager::viewSummary() {
    long long total = 0;
    for (const auto& e : expenses) {
        total += e.amount;
    }
    std::cout << "total expense: $" << total << std::endl;
    if(expenses.empty()) {
        std::cout << "no expenses added yet. \n";
        return;
    }

}

void listCommands() {
    std::cout << "1. add <Description> <Amount> <Category> \n"
        << "2. view\n" <<"3. update\n" << "4. delete\n" << "5. viewsum\n";
}

void ExpenseManager::deleteExpense(int id) {
    for (auto it = expenses.begin(); it != expenses.end(); ++it) {
        if (it -> ID == id) {
            expenses.erase(it);
            std::cout << "expense deleted.\n";
            return;
        }
    }
    std::cout << "ID not found.\n";
}

void ExpenseManager::saveToFile(const std::string& filename) const{
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file for saving." << std::endl;
        return;
    }

    file << "ID,Description,Amount,Category,Timestamp\n";

    for (const auto& e : expenses) {
        file << e.ID << ','
        <<std::quoted(e.description) << ','
        << e.amount << ','
        << std::quoted(e.category) << ','
        <<e.getTimeString() << '\n';
    }
    file.close();
    std::cout << "Expenses saved to file." << std::endl;
}

void ExpenseManager::loadFromFile(const std::string& filename)  {
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // Skip header line

    int maxID = 0;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string idStr, desc, amtStr, cat, timeStr;
        

        std::getline(iss, idStr,',');
        iss >> std::quoted(desc);
        iss.ignore();
        std::getline(iss, amtStr, ',');
        iss >> std::quoted(cat);
        iss.ignore();
        std::getline(iss, timeStr);

        int id = std::stoi(idStr); 
        long long amount = std::stoll(amtStr);

        std::tm tm = {};
        std::istringstream ss(timeStr);
        ss >> std::get_time(&tm,"%Y-%m-%d %H:%M:%S");
        std::time_t tt = std::mktime(&tm);
        auto timestamp = std::chrono::system_clock::from_time_t(tt);


        Expense e(id, desc, amount, cat);
        e.timestamp = timestamp;
        expenses.push_back(e);

        if (id > maxID) {
            maxID = id;
        }
    }
    nextID = maxID + 1;
}

int main() {
    ExpenseManager manager;
    std::cout << ">ex// type 'help' to view commands\n";
    std::string input;
    
    while(true) {
        std::cout << ">ex// "; //newline indicator
        std::getline(std::cin, input);
        std::stringstream ss(input);
        std::string command;
        ss >> command;

        if (command == "help") {
            listCommands();
        }
        else if (command == "exit") break;

        else if (command == "add") {

            std::string line = input.substr(4);
            std::stringstream ss(line);
            
            std::string word;
            std::vector<std::string> tokens;

            while (ss >> word) {
                tokens.push_back(word);
            }

            if (tokens.size() < 3) {
                std::cout << "invalid input. Format : <description> <amount> <category> \n";
                continue;
            }

            std::string category = tokens.back();
            tokens.pop_back();

            long long amount = std::stoll(tokens.back());
            tokens.pop_back();

            std::string description;

            for (const auto& w : tokens) description += w + " ";
            description.pop_back(); 


            manager.addExpense(description,amount,category);
        }
        else if (command == "view") {
            manager.listExpense();
        }
        else if (command == "update") {
            int id;
            manager.listExpense();
            std::cout << "enter ID to update: ";
            std::cin >> id;
            manager.updateExpense(id);
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else if (command == "delete") {
            int id;
            std::cout << "enter ID to delete: ";
            std::cin >> id;
            manager.deleteExpense(id);
            std::cin.ignore();
        }
        else if (command == "viewsum") {
            manager.viewSummary();
        }
        else if (command == "load") {
            std::string filename;
            std::cout << "Enter filename to load: ";
            std::getline(std::cin, filename);
            manager.loadFromFile(filename);
        }
        
        else if (command == "save") {
            std::string filename;
            std::cout << "Enter filename to save: ";
            std::cin >> filename;
            manager.saveToFile(filename);
        }
        else {
            std::cout << "command unknown. Type 'help' to view commands\n";
        }
    }

    manager.listExpense();
    
    return 0;
}