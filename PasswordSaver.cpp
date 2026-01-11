#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <windows.h>
#include <conio.h>

using namespace std;

const string DATA_FILE = "vault.dat";
const string MASTER_FILE = "master.key";

const int COLOR_GRAY = 8;
const int COLOR_GREEN = 10;
const int COLOR_CYAN = 11;
const int COLOR_RED = 12;
const int COLOR_YELLOW = 14;
const int COLOR_DEFAULT = 15;

struct Entry
{
    string title;
    int type;
    string password;
    time_t modifiedAt;
    int vault;
};

vector<Entry> entries;
string masterKey;

void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printHeader(const string &text)
{
    setColor(COLOR_CYAN);
    cout << "\n  ";
    for (int i = 0; i < 40; i++)
        cout << "=";
    cout << "\n  ";
    int pad = (38 - text.length()) / 2;
    for (int i = 0; i < pad; i++)
        cout << " ";
    cout << text << "\n  ";
    for (int i = 0; i < 40; i++)
        cout << "=";
    cout << "\n\n";
    setColor(COLOR_DEFAULT);
}

void pauseScreen()
{
    setColor(COLOR_GRAY);
    cout << "\n  Press any key to continue...";
    setColor(COLOR_DEFAULT);
    _getch();
}

string maskedInput(const string &prompt)
{
    setColor(COLOR_CYAN);
    cout << prompt;
    setColor(COLOR_YELLOW);

    string input;
    char ch;
    bool show = false;

    while ((ch = _getch()) != '\r')
    {
        if (ch == '\t')
        {
            show = !show;
            setColor(COLOR_CYAN);
            cout << "\r" << prompt;
            setColor(COLOR_YELLOW);

            for (int i = 0; i < input.size(); i++)
                cout << (show ? input[i] : '*');

            continue;
        }

        if (ch == '\b')
        {
            if (!input.empty())
            {
                input.pop_back();
                cout << "\b \b";
            }
        }
        else if (ch >= 32 && ch <= 126)
        {
            input += ch;
            cout << (show ? ch : '*');
        }
    }

    cout << endl;
    setColor(COLOR_DEFAULT);
    return input;
}

string getTypeString(int type)
{
    if (type == 1)
        return "Pattern";
    if (type == 2)
        return "PIN";
    return "Text";
}

string toLowercase(const string &s)
{
    string result = s;
    for (int i = 0; i < result.size(); i++)
        result[i] = tolower(result[i]);
    return result;
}

string encryptdecrypt(string data, const string &key)
{
    for (int i = 0; i < data.size(); i++)
    {
        data[i] ^= key[i % key.size()];
    }
    return data;
}

void saveData()
{
    ofstream outFile(DATA_FILE, ios::binary | ios::trunc);
    if (!outFile)
    {
        setColor(COLOR_RED);
        cout << "Error saving data file.";
        setColor(COLOR_DEFAULT);
        return;
    }

    string rawData = "";
    for (int i = 0; i < entries.size(); i++)
    {
        Entry &e = entries[i];
        rawData += e.title + "|";
        rawData += to_string(e.type) + "|";
        rawData += e.password + "|";
        rawData += to_string(e.modifiedAt) + "|";
        rawData += to_string(e.vault) + "\n";
    }

    string encryptedData = encryptdecrypt(rawData, masterKey);
    outFile.write(encryptedData.c_str(), encryptedData.size());
    outFile.close();
}

void loadData()
{
    ifstream in(DATA_FILE, ios::binary);
    if (!in)
    {
        return;
    }

    string encrypted = "";
    char ch;
    while (in.get(ch))
        encrypted += ch;
    in.close();

    if (encrypted.empty())
        return;

    string rawData = encryptdecrypt(encrypted, masterKey);
    entries.clear();

    string line = "";
    for (int i = 0; i < rawData.size(); i++)
    {
        if (rawData[i] == '\n')
        {
            vector<string> parts;
            string temp = "";

            for (int j = 0; j < line.size(); j++)
            {
                if (line[j] == '|')
                {
                    parts.push_back(temp);
                    temp = "";
                }
                else
                {
                    temp += line[j];
                }
            }
            // FIX: Push the last part (vault) that doesn't end with |
            if (!temp.empty())
            {
                parts.push_back(temp);
            }

            if (parts.size() == 5)
            {
                Entry e;
                e.title = parts[0];
                e.type = stoi(parts[1]);
                e.password = parts[2];
                e.modifiedAt = stoll(parts[3]);
                e.vault = stoi(parts[4]);
                entries.push_back(e);
            }

            line = "";
        }
        else
        {
            line += rawData[i];
        }
    }
}

bool masterExists()
{
    ifstream f(MASTER_FILE);
    bool exists = f.good() && f.peek() != EOF;
    f.close();
    return exists;
}

void saveMaster(const string &pass)
{
    ofstream f(MASTER_FILE, ios::binary);
    string enc = encryptdecrypt(pass, "V@ultK3y#2024");
    f.write(enc.c_str(), enc.size());
    f.close();
}

bool checkMaster(const string &pass)
{
    ifstream f(MASTER_FILE, ios::binary);
    string stored;
    char c;
    while (f.get(c))
        stored += c;
    f.close();
    stored = encryptdecrypt(stored, "V@ultK3y#2024");
    return stored == pass;
}

bool isInPattern(const string &pattern, char c)
{
    for (int i = 0; i < pattern.size(); i++)
    {
        if (pattern[i] == c)
            return true;
    }
    return false;
}

void drawInputGrid(const string &sequence)
{
    system("cls");
    printHeader("PATTERN INPUT");

    setColor(COLOR_GRAY);
    cout << "  Press 1-9 to draw. Enter to save. Backspace to undo.\n\n";
    setColor(COLOR_DEFAULT);

    for (int row = 0; row < 3; row++)
    {
        cout << "\t    ";
        for (int col = 0; col < 3; col++)
        {
            int num = row * 3 + col + 1;
            char c = '0' + num;

            if (isInPattern(sequence, c))
            {
                setColor(COLOR_GREEN);
                cout << " [" << num << "] ";
            }
            else
            {
                setColor(COLOR_GRAY);
                cout << " (" << num << ") ";
            }
        }
        cout << "\n\n";
    }
    setColor(COLOR_DEFAULT);

    if (!sequence.empty())
    {
        setColor(COLOR_YELLOW);
        cout << "\n  Pattern: ";
        for (int i = 0; i < sequence.size(); i++)
        {
            cout << sequence[i];
            if (i < sequence.size() - 1)
                cout << " -> ";
        }
        cout << "\n";
        setColor(COLOR_DEFAULT);
    }
}

void drawAnimGrid(const string &sequence)
{
    system("cls");
    printHeader("PATTERN REPLAY");

    for (int row = 0; row < 3; row++)
    {
        cout << "\t    ";
        for (int col = 0; col < 3; col++)
        {
            int num = row * 3 + col + 1;
            char c = '0' + num;

            if (isInPattern(sequence, c))
            {
                setColor(COLOR_GREEN);
                cout << " [O] ";
            }
            else
            {
                setColor(COLOR_GRAY);
                cout << "  .  ";
            }
        }
        cout << "\n\n";
    }
    setColor(COLOR_DEFAULT);
}

string getPattern()
{
    string p;
    while (true)
    {
        drawInputGrid(p);

        char ch = _getch();

        if (ch == '\r')
        {
            if (p.length() >= 2)
            {
                break;
            }
            else
            {
                setColor(COLOR_RED);
                cout << "\n  Pattern must have at least 2 points!";
                setColor(COLOR_DEFAULT);
                Sleep(1000);
            }
        }
        else if (ch == '\b' && !p.empty())
        {
            p.pop_back();
        }
        else if (ch >= '1' && ch <= '9')
        {
            p += ch;
        }
    }
    return p;
}

void animatePattern(const string &p)
{
    string part;
    for (int i = 0; i < p.size(); i++)
    {
        part += p[i];
        drawAnimGrid(part);

        // Show progress
        setColor(COLOR_YELLOW);
        cout << "\n  Step " << (i + 1) << " of " << p.size();
        setColor(COLOR_DEFAULT);

        Sleep(500);
    }

    // Final display
    drawAnimGrid(p);
    setColor(COLOR_GREEN);
    cout << " Pattern complete!";
    setColor(COLOR_DEFAULT);
    pauseScreen();
}

string getConfirmedPassword(int type)
{
    while (true)
    {
        if (type == 1)
            return getPattern();
        cout << "  [TAB] to toggle show/hide password\n";
        if (type == 2)
        {
            string pin1 = maskedInput("  Enter PIN: ");
            string pin2 = maskedInput("  Confirm PIN: ");
            if (pin1 == pin2)
                return pin1;
            else
            {
                setColor(COLOR_RED);
                cout << "  PINs do not match! Try again.\n";
                setColor(COLOR_DEFAULT);
            }
        }
        else
        {
            string pass1 = maskedInput("  Enter Password: ");
            string pass2 = maskedInput("  Confirm Password: ");
            if (pass1 == pass2)
                return pass1;
            else
            {
                setColor(COLOR_RED);
                cout << "  Passwords do not match! Try again.\n";
                setColor(COLOR_DEFAULT);
            }
        }
    }
}

string getPassword(int type)
{
    if (type == 1)
        return getPattern();

    cout << "  [TAB] to toggle show/hide password\n";
    if (type == 2)
        return maskedInput("  Enter PIN: ");
    return maskedInput("  Enter Password: ");
}

void copyToClipboard(const string &text)
{
    if (OpenClipboard(NULL))
    {
        EmptyClipboard();
        HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
        if (h)
        {
            memcpy(GlobalLock(h), text.c_str(), text.size() + 1);
            GlobalUnlock(h);
            SetClipboardData(CF_TEXT, h);
        }
        CloseClipboard();

        setColor(COLOR_GREEN);
        cout << ("Copied to clipboard!");
        setColor(COLOR_YELLOW);
        cout << "  Clearing in 10 seconds...\n";
        setColor(COLOR_DEFAULT);

        Sleep(10000);

        // Clear clipboard
        if (OpenClipboard(NULL))
        {
            EmptyClipboard();
            CloseClipboard();
        }
        cout << "Clipboard cleared.";
    }
    else
    {
        setColor(COLOR_RED);
        cout << "Failed to copy to clipboard.";
        setColor(COLOR_DEFAULT);
    }
}

void addEntry(int vault)
{
    system("cls");
    printHeader(vault == 1 ? "ADD TO MY PASSWORDS" : "ADD TO FRIEND'S PASSWORDS");

    Entry e;
    e.vault = vault;

    cout << "  Title: ";
    setColor(COLOR_CYAN);
    // Correct: Do NOT use cin.ignore() here because _getch() was used in menu
    getline(cin, e.title);
    setColor(COLOR_DEFAULT);

    if (e.title.empty())
    {
        setColor(COLOR_RED);
        cout << "  Title cannot be empty!";
        setColor(COLOR_DEFAULT);
        pauseScreen();
        return;
    }

    cout << "\n  Password Type:\n";
    setColor(COLOR_YELLOW);
    cout << "  1. Pattern (3x3 grid)\n";
    cout << "  2. PIN (numeric)\n";
    cout << "  3. Text Password\n";
    setColor(COLOR_DEFAULT);
    cout << "\n  Choice> ";

    char typeCh = _getch();
    cout << typeCh << endl;

    if (typeCh < '1' || typeCh > '3')
    {
        setColor(COLOR_RED);
        cout << "Invalid type!";
        setColor(COLOR_DEFAULT);
        pauseScreen();
        return;
    }

    e.type = typeCh - '0';

    string pass = getConfirmedPassword(e.type);
    if (pass.empty())
    {
        setColor(COLOR_RED);
        cout << "Password cannot be empty!";
        setColor(COLOR_DEFAULT);
        pauseScreen();
        return;
    }

    e.password = encryptdecrypt(pass, masterKey);
    e.modifiedAt = time(0);

    entries.push_back(e);
    saveData();

    system("cls");
    printHeader("SAVED");
    setColor(COLOR_GREEN);
    cout << "  Entry saved successfully!\n";
    // Show summary
    setColor(COLOR_DEFAULT);
    cout << "  Title: ";
    setColor(COLOR_CYAN);
    cout << e.title << "\n";
    setColor(COLOR_DEFAULT);
    cout << "  Type:  ";
    setColor(COLOR_YELLOW);
    cout << getTypeString(e.type) << "\n";
    setColor(COLOR_DEFAULT);

    pauseScreen();
}

void updateEntry(Entry *e)
{
    system("cls");
    printHeader("UPDATE ENTRY");

    setColor(COLOR_DEFAULT);
    cout << "  Current Title: ";
    setColor(COLOR_CYAN);
    cout << e->title << "\n\n";
    setColor(COLOR_DEFAULT);

    cout << "  What to update?\n";
    setColor(COLOR_YELLOW);
    cout << "  1. Title\n";
    cout << "  2. Password\n";
    cout << "  3. Type & Password\n";
    cout << "  4. Cancel\n";
    setColor(COLOR_DEFAULT);
    cout << "\n  Choice> ";
    setColor(COLOR_DEFAULT);

    char ch = _getch();
    cout << ch << endl;

    if (ch == '1')
    {
        setColor(COLOR_DEFAULT);
        cout << "\n  New Title: ";
        setColor(COLOR_CYAN);
        string newTitle;
        // FIX: Removed cin.ignore() which was eating the first character
        getline(cin, newTitle);
        setColor(COLOR_DEFAULT);

        if (!newTitle.empty())
        {
            e->title = newTitle;
        }
    }
    else if (ch == '2')
    {
        cout << endl;
        string pass = getPassword(e->type);
        if (!pass.empty())
        {
            e->password = encryptdecrypt(pass, masterKey);
        }
    }
    else if (ch == '3')
    {
        cout << "\n  New Type:\n";
        setColor(COLOR_YELLOW);
        cout << "  1. Pattern\n";
        cout << "  2. PIN\n";
        cout << "  3. Text\n";
        setColor(COLOR_DEFAULT);
        cout << "\n  Choice> ";
        setColor(COLOR_DEFAULT);

        char typeCh = _getch();
        cout << typeCh << endl;

        if (typeCh >= '1' && typeCh <= '3')
        {
            e->type = typeCh - '0';
            string pass = getPassword(e->type);
            if (!pass.empty())
            {
                e->password = encryptdecrypt(pass, masterKey);
            }
        }
    }
    else if (ch == '4')
    {
        return;
    }

    e->modifiedAt = time(0);
    saveData();
    setColor(COLOR_GREEN);
    cout << " Entry updated!";
    pauseScreen();
}

void showEntry(Entry *e)
{
    bool show = false;

    while (true)
    {
        system("cls");
        printHeader("PASSWORD DETAILS");

        // Title
        setColor(COLOR_DEFAULT);
        cout << "  Title:     ";
        setColor(COLOR_CYAN);
        cout << e->title << "\n";

        // Type
        setColor(COLOR_DEFAULT);
        cout << "  Type:      ";
        setColor(COLOR_YELLOW);
        cout << getTypeString(e->type) << "\n";

        // Password
        string dec = encryptdecrypt(e->password, masterKey);
        setColor(COLOR_DEFAULT);
        cout << "  Password:  ";

        if (show)
        {
            setColor(COLOR_GREEN);
            if (e->type == 1)
            {
                cout << "[Pattern - use Replay]\n";
            }
            else
            {
                cout << dec << "\n";
            }
        }
        else
        {
            setColor(COLOR_GRAY);
            cout << "********\n";
        }

        char timeStr[64];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&e->modifiedAt));
        setColor(COLOR_DEFAULT);
        cout << "  Last Modified: ";
        setColor(COLOR_GRAY);
        cout << timeStr << "\n";

        setColor(COLOR_DEFAULT);

        // Options
        cout << "\n  Options:\n";
        setColor(COLOR_YELLOW);
        cout << "  1. " << (show ? "Hide" : "Show") << " Password\n";

        if (e->type == 1)
            cout << "  2. Replay Pattern\n";
        else
            cout << "  2. Copy to Clipboard\n";

        cout << "  3. Update\n";
        cout << "  4. Delete\n";
        cout << "  5. Back\n";

        setColor(COLOR_DEFAULT);
        cout << "\n  Choice> ";
        setColor(COLOR_DEFAULT);

        char ch = _getch();
        cout << ch << endl;

        if (ch == '1')
        {
            show = !show;
        }
        else if (ch == '2')
        {
            if (e->type == 1)
            {
                animatePattern(dec);
            }
            else
            {
                copyToClipboard(dec);
                pauseScreen();
            }
        }
        else if (ch == '3')
        {
            updateEntry(e);
        }
        else if (ch == '4')
        {
            setColor(COLOR_RED);
            cout << "\n  Are you sure you want to delete? (y/n): ";
            setColor(COLOR_DEFAULT);

            char confirm = _getch();
            cout << confirm << endl;

            if (confirm == 'y' || confirm == 'Y')
            {
                for (int i = 0; i < entries.size(); i++)
                {
                    if (&entries[i] == e)
                    {
                        entries.erase(entries.begin() + i);
                        break;
                    }
                }
                saveData();
                setColor(COLOR_GREEN);
                cout << "Entry deleted!";
                pauseScreen();
                return;
            }
        }
        else if (ch == '5')
        {
            return;
        }
    }
}

void listVault(int vault)
{
    while (true)
    {
        system("cls");
        printHeader(vault == 1 ? "MY PASSWORDS" : "FRIEND'S PASSWORDS");

        vector<Entry *> v;
        for (int i = 0; i < entries.size(); i++)
        {
            if (entries[i].vault == vault)
                v.push_back(&entries[i]);
        }

        if (v.empty())
        {
            setColor(COLOR_RED);
            cout << "  No entries found.";
            pauseScreen();
            return;
        }

        setColor(COLOR_DEFAULT);
        cout << "  Found " << v.size() << " entry(s):\n\n";

        for (int i = 0; i < v.size(); i++)
        {
            setColor(COLOR_DEFAULT);
            cout << "  " << (i + 1) << ". ";
            setColor(COLOR_CYAN);
            cout << v[i]->title;
            setColor(COLOR_YELLOW);
            cout << " [" << getTypeString(v[i]->type) << "]";
            setColor(COLOR_DEFAULT);
            cout << "\n";
        }

        setColor(COLOR_GRAY);
        cout << "\n  0. Back\n";
        setColor(COLOR_DEFAULT);
        cout << "\n  Choice: ";
        setColor(COLOR_DEFAULT);

        int sel;
        cin >> sel;
        // Keep cin.ignore() here because 'cin >>' was used, which leaves a newline
        cin.ignore();

        if (sel == 0)
            return;
        if (sel > 0 && sel <= v.size())
            showEntry(v[sel - 1]);
    }
}

void searchVault(int vault)
{
    system("cls");
    printHeader("SEARCH");

    setColor(COLOR_DEFAULT);
    cout << "  Enter search term: ";
    setColor(COLOR_CYAN);

    string q;
    // FIX: Removed cin.ignore() because _getch() was used in menu
    getline(cin, q);
    setColor(COLOR_DEFAULT);

    if (q.empty())
    {
        setColor(COLOR_RED);
        cout << "Search cancelled.";
        setColor(COLOR_DEFAULT);
        pauseScreen();
        return;
    }

    vector<Entry *> matches;
    for (int i = 0; i < entries.size(); i++)
    {
        if (entries[i].vault == vault)
        {
            string lowerTitle = toLowercase(entries[i].title);
            string lowerQuery = toLowercase(q);
            if (lowerTitle.find(lowerQuery) != string::npos)
            {
                matches.push_back(&entries[i]);
            }
        }
    }

    if (matches.empty())
    {
        setColor(COLOR_RED);
        cout << "  No matches found for: " + q + "\n";
        setColor(COLOR_DEFAULT);
        pauseScreen();
        return;
    }

    cout << "\n";
    setColor(COLOR_GREEN);
    cout << "  Found " << matches.size() << " match(es):\n";
    cout << "\n";

    for (int i = 0; i < matches.size(); i++)
    {
        setColor(COLOR_DEFAULT);
        cout << "  " << (i + 1) << ". ";
        setColor(COLOR_CYAN);
        cout << matches[i]->title;
        setColor(COLOR_YELLOW);
        cout << " [" << getTypeString(matches[i]->type) << "]";
        setColor(COLOR_DEFAULT);
        cout << "\n";
    }

    cout << "\n  Enter number to view (0 to go back): ";

    int choice;
    cin >> choice;
    cin.ignore();

    if (choice >= 1 && choice <= matches.size())
    {
        showEntry(matches[choice - 1]);
    }
}

// Comparison functions
bool compareAZ(const Entry &a, const Entry &b)
{
    return toLowercase(a.title) < toLowercase(b.title);
}

bool compareZA(const Entry &a, const Entry &b)
{
    return toLowercase(a.title) > toLowercase(b.title);
}

bool compareNewest(const Entry &a, const Entry &b)
{
    return a.modifiedAt > b.modifiedAt;
}

bool compareOldest(const Entry &a, const Entry &b)
{
    return a.modifiedAt < b.modifiedAt;
}

void sortVault(int vault)
{
    system("cls");
    printHeader("SORT ENTRIES");

    cout << "  Sort by:\n";
    setColor(COLOR_YELLOW);
    cout << "  1. A to Z\n";
    cout << "  2. Z to A\n";
    cout << "  3. Newest first\n";
    cout << "  4. Oldest first\n";
    cout << "  5. Cancel\n";
    setColor(COLOR_DEFAULT);
    cout << "\n  Choice> ";
    setColor(COLOR_DEFAULT);

    char c = _getch();
    cout << c << endl;

    if (c >= '1' && c <= '4')
    {
        // FIX: Sort only entries of this vault
        vector<Entry> vaultEntries;
        vector<Entry> otherEntries;

        for (int i = 0; i < entries.size(); i++)
        {
            if (entries[i].vault == vault)
                vaultEntries.push_back(entries[i]);
            else
                otherEntries.push_back(entries[i]);
        }

        if (c == '1')
            sort(vaultEntries.begin(), vaultEntries.end(), compareAZ);
        else if (c == '2')
            sort(vaultEntries.begin(), vaultEntries.end(), compareZA);
        else if (c == '3')
            sort(vaultEntries.begin(), vaultEntries.end(), compareNewest);
        else if (c == '4')
            sort(vaultEntries.begin(), vaultEntries.end(), compareOldest);

        // Rebuild entries
        entries.clear();
        for (int i = 0; i < vaultEntries.size(); i++)
            entries.push_back(vaultEntries[i]);
        for (int i = 0; i < otherEntries.size(); i++)
            entries.push_back(otherEntries[i]);

        saveData();
        setColor(COLOR_GREEN);
        cout << " Entries sorted!\n";
        setColor(COLOR_DEFAULT);
        pauseScreen();
    }
}

void vaultMenu(int vault)
{
    while (true)
    {
        system("cls");
        printHeader(vault == 1 ? "MY PASSWORDS" : "FRIEND'S PASSWORDS");

        int count = 0;
        for (int i = 0; i < entries.size(); i++)
        {
            if (entries[i].vault == vault)
                count++;
        }

        setColor(COLOR_DEFAULT);
        cout << "  Total entries: ";
        setColor(COLOR_CYAN);
        cout << count << "\n\n";
        setColor(COLOR_YELLOW);
        cout << "  1. New Entry\n";
        cout << "  2. List All\n";
        cout << "  3. Search\n";
        cout << "  4. Sort\n";
        cout << "  5. Back\n";
        setColor(COLOR_DEFAULT);
        cout << "\n  Choice> ";

        char ch = _getch();
        cout << ch << endl;

        if (ch == '1')
            addEntry(vault);
        else if (ch == '2')
            listVault(vault);
        else if (ch == '3')
            searchVault(vault);
        else if (ch == '4')
            sortVault(vault);
        else if (ch == '5')
            break;
    }
}

int main()
{
    system("cls");
    printHeader("PASSWORD VAULT");

    // Setup or Login
    if (!masterExists())
    {
        setColor(COLOR_GREEN);
        cout << "  Welcome! First time setup.\n\n";
        setColor(COLOR_YELLOW);
        cout << "  IMPORTANT: Remember your master password!\n";
        cout << "  If forgotten, your data cannot be recovered.\n\n";
        setColor(COLOR_DEFAULT);

        string pass = maskedInput("  Create Master Password: ");

        if (pass.length() < 4)
        {
            setColor(COLOR_RED);
            cout << "Password must be at least 4 characters!\n";
            pauseScreen();
            return 0;
        }

        saveMaster(pass);
        masterKey = pass;
        setColor(COLOR_GREEN);
        cout << "\n  Master password set successfully!\n";
        setColor(COLOR_DEFAULT);
        pauseScreen();
    }
    else
    {
        int tryCount = 0;

        while (tryCount < 3)
        {
            string pass = maskedInput("  Enter Master Password: ");

            if (checkMaster(pass))
            {
                masterKey = pass;
                cout << "  Login successful!\n";
                Sleep(800);
                break;
            }
            else
            {
                tryCount++;
                setColor(COLOR_RED);
                cout << "Incorrect password!\n";

                if (tryCount < 3)
                {
                    setColor(COLOR_YELLOW);
                    cout << "  " << (3 - tryCount) << " attempt(s) remaining.\n";
                    setColor(COLOR_DEFAULT);
                }
                pauseScreen();
                system("cls");
                printHeader("PASSWORD VAULT");
            }
        }

        if (tryCount >= 3)
        {
            setColor(COLOR_RED);
            cout << "Too many failed attempts. Exiting.\n";
            setColor(COLOR_DEFAULT);
            Sleep(2000);
            return 0;
        }
    }

    while (true)
    {
        system("cls");
        loadData();

        printHeader("MAIN MENU");

        int myCount = 0, friendCount = 0;
        for (int i = 0; i < entries.size(); i++)
        {
            if (entries[i].vault == 1)
                myCount++;
            else
                friendCount++;
        }

        setColor(COLOR_YELLOW);
        cout << "  1. My Passwords";
        setColor(COLOR_CYAN);
        cout << " (" << myCount << ")\n";

        setColor(COLOR_YELLOW);
        cout << "  2. Friend's Passwords";
        setColor(COLOR_CYAN);
        cout << " (" << friendCount << ")\n";

        setColor(COLOR_YELLOW);
        cout << "  3. Exit\n";

        setColor(COLOR_DEFAULT);
        cout << "\n  Choice> ";
        setColor(COLOR_DEFAULT);

        char ch = _getch();
        cout << ch << endl;

        if (ch == '1')
            vaultMenu(1);
        else if (ch == '2')
            vaultMenu(2);
        else if (ch == '3')
        {
            system("cls");
            printHeader("GOODBYE");
            Sleep(1500);
            break;
        }
    }

    return 0;
}