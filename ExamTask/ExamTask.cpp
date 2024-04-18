#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <vector>
using namespace std;

/*
  Система управления персональными финансами
  Кошельки и карты, которыми можно управлять с помощью этой системы, существуют независимо от системы управления.
  Поэтому в программе реализована слабая связь между системой управления персональными финансами и "хранилищами денег" (кошельками и картами)
*/

class Spending
{
	double money;
	string category;  
	time_t timestamp;
public:
	Spending(double money, const string& category, time_t timestamp) : money(money), category(category), timestamp(timestamp) {}
	double getMoney() const 
	{
		return money;
	}
	string getCategory() const 
	{
		return category;
	}
	time_t getTimestamp() const 
	{
		return timestamp;
	}
};

class Wallet 
{ 
protected:
	string name;
	long long number;
	int password;
	double balance;

public:
	Wallet(const string& name,long long number,int password) : name(name), balance(0.0), number(number), password(password) {}
	virtual ~Wallet() {}
	string getName() const 
	{
		return name;
	}
	long long getNumber() const
	{
		return number;
	}
	int getPassword() const
	{
		return password;
	}
	double getBalance() const 
	{
		return balance;
	}
	virtual void deposit(double amount) = 0;
	virtual void addSpending(double amount, const string& category) = 0;
	virtual void printSummary() const = 0;
};

class VirtualCard : public Wallet
{
public:
	VirtualCard(const string& name,long long number,int password) : Wallet(name,number,password) {}

	void deposit(double amount) override 
	{
		balance += amount;
	}
	void addSpending(double amount, const string& category) override 
	{
		balance -= amount;
	}
	void printSummary() const override 
	{
		cout << "Virtual Wallet: " << name << endl;
		cout << "Number of Wallet: " << number << endl;
		cout << "Balance: " << balance << endl;
	}
};

class Card : public Wallet 
{ 
private:
	string cardType;
public:
	Card(const string& name,long long number,int password, const string& cardType) : Wallet(name,number,password), cardType(cardType) {}
	void deposit(double amount) override 
	{
		balance += amount;
	}
	void addSpending(double amount, const string& category) override
	{
		balance -= amount;
	}
	void printSummary() const override 
	{
		cout << "Name of " << cardType << " Card: " << name << endl;
		cout << "Number of Wallet: " << number << endl;
		cout << "Balance: " << balance << endl;
	}
};


class FinanceManagement
{
	vector<Wallet*> wallets;
	map<long long, vector<Spending>> spend;
	map<long long, map<time_t, double>> timeSpending; //статистика затрат 
	void addTimestampToSpending(const long long& walletNumber,const Spending& spend) //добавление времени затрат в статистику
	{ 
		time_t timestamp = spend.getTimestamp();
		timeSpending[walletNumber][timestamp] += spend.getMoney();
	}

	Wallet* findWallet(long long& number) //Поиск кошелька 
	{
		for (Wallet* wallet : wallets)
		{
			if (wallet->getNumber() == number)
			{
				return wallet;
			}
		}
		cout << "Wallet was not founded." << endl;
		return nullptr;
	}
	void saveSpending(const long long& wallerNumber) 
	{ 
		ofstream inputFile("Spending.txt");
		if (!inputFile)
		{
			cout << "Error opening file." << endl;
			return;
		}
		for (const auto& spending : spend) 
		{
			const vector<Spending>& spendingList = spending.second;
			for (const Spending& expense : spendingList) 
			{
				time_t timestamp = expense.getTimestamp();
				inputFile << "Spending" << endl;
				inputFile << "----------------" << endl;
				inputFile << "Number of waller: " << wallerNumber << endl;
				inputFile << "Amount: " << expense.getMoney() << endl;
				inputFile << "Category: " << expense.getCategory() << endl;
				inputFile << "Timestamp: " << ctime(&timestamp);
				inputFile << "----------------" << endl;
			}
		}
		inputFile.close();
	}
public:
	FinanceManagement() {};
	~FinanceManagement()
	{
		for (Wallet* wallet : wallets) 
		{
			delete wallet;
		}
	}
	void createWallet() //Создание кошелька
	{ 
		int walletType;
		cout << "Select wallet type:" << endl;
		cout << "1. Virtual Card" << endl;
		cout << "2. Debit Card" << endl;
		cout << "3. Credit Card" << endl;
		cout << "Enter wallet type: ";
		cin >> walletType;
		string walletName;
		cout << "Enter wallet name: ";
		cin >> walletName;
		long long walletNumber;
		while (true) 
		{
			cout << "Enter a card number (16 digits): ";
			cin >> walletNumber;
			if (to_string(walletNumber).length() == 16) 
			{
				cout << "Successfully!" << endl;
				break;
			}
			else {
				cout << "Invalid card number! Your card must be 16 digits long. Please try again." << endl;
			}
		}
		int password;
		cout << "Enter a password: ";
		cin >> password;

		Wallet* newWallet = nullptr;
		switch (walletType) {
		case 1:
			newWallet = new Card(walletName,walletNumber,password, "Debit");
			break;
		case 2:
			newWallet = new Card(walletName,walletNumber,password, "Credit");
			break;
		case 3:
			newWallet = new VirtualCard(walletName,walletNumber,password);
			break;
		default:
			cout << "Invalid wallet type." << endl;
			return;
		}
		if (newWallet != nullptr) 
		{
			wallets.push_back(newWallet);
			cout << "Wallet created successfully." << endl;
		}
	}
	void replenishingWallet() //Пополнение кошелька
	{ 
		long long walletNumber;
		while (true)
		{
			cout << "Enter a card number (16 digits): ";
			cin >> walletNumber;
			if (to_string(walletNumber).length() == 16) 
			{
				cout << "Successfully!" << endl;
				break;
			}
			else 
			{
				cout << "Invalid card number! Your card must be 16 digits long. Please try again." << endl;
			}
		}
		double amount;
		cout << "Enter amount to replenish: ";
		cin >> amount;
		Wallet* wallet = findWallet(walletNumber);
		if (wallet != nullptr) 
		{
			wallet->deposit(amount);
			cout << "Replenishment was successful." << endl;
		}
		else 
		{
			cout << "Wallet not found." << endl;
		}
	}
	void addSpending() //Добавление затрат
	{ 
		long long walletNumber;
		while (true)
		{
			cout << "Enter a card number (16 digits): ";
			cin >> walletNumber;
			if (to_string(walletNumber).length() == 16) 
			{
				cout << "Successfully!" << endl;
				break;
			}
			else 
			{
				cout << "Invalid card number! Your card must be 16 digits long. Please try again." << endl;
			}
		}
		double amount;
		cout << "Enter spending amount: ";
		cin >> amount;
		string category;
		cout << "Enter spending category: ";
		cin >> category;
		Wallet* wallet = findWallet(walletNumber);
		if (wallet != nullptr) 
		{
			wallet->addSpending(amount, category);
			time_t timestamp = time(nullptr);
			Spending expense(amount, category, timestamp);
			spend[walletNumber].push_back(expense);
			addTimestampToSpending(walletNumber, expense);
			cout << "Spending added successfully." << endl;
		}
		else 
		{
			cout << "Wallet not found." << endl;
		}
		saveSpending(walletNumber);
	}
	void printInfoAboutWallet() // Вывод информации о кошельке
	{ 
		long long number;
		cout << "Enter wallet number: ";
		cin >> number;
		cout << endl;
		Wallet* wallet = findWallet(number);
		if (wallet != nullptr) {
			wallet->printSummary();
		}
		else {
			cout << "Wallet not found." << endl;
		}
	}
	void printInfoAboutSpending() //Вывод информации о затратах
	{
		ifstream outputFile("Spending.txt");
		string line;
		if (!outputFile)
		{
			cout << "Error opening file." << endl;
			return;
		}
		while (getline(outputFile, line))
		{
			cout << line << endl;
		}
	}
};

int main()
{	
	FinanceManagement manager;
	while (true) 
	{
		cout << "Finance Manager" << endl;
		cout << "1. Create Wallet" << endl;
		cout << "2. Replenish to Wallet" << endl;
		cout << "3. Add spending" << endl;
		cout << "4. Print information about wallet" << endl;
		cout << "5. Print information about spending" << endl;
		cout << "6. Exit" << endl;
		int choice;
		cout << "Enter your choice: ";
		cin >> choice;
		cout << endl;
		switch (choice)
		{ 
		case 1:
			manager.createWallet();
			break;
		case 2:
			manager.replenishingWallet();
			break;
		case 3:
			manager.addSpending();
			break;
		case 4:
			manager.printInfoAboutWallet();
			break;
		case 5:
			manager.printInfoAboutSpending();
			break;
		case 6:
			cout << "Exiting program." << endl;
			return 0;
		default:
			cout << "Invalid choice." << endl;
			break;
		}
		cout << endl;
	}
}

