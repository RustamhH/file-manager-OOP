#pragma once




// Item Class --> abstract class

class Item
{
	string _name = "";
	int _attributes = 0;
	size_t _size = 0;
public:
	// Pure Virtual Methods
	virtual void Create(string newName) = 0;
	virtual void Print() = 0 {
		cout << "Name: " << _name << endl;
		cout << "Attributes: " << _attributes << endl;
		cout << "Size: " << _size << endl;
	};
	virtual void Rename(string newName) = 0;
	virtual void Delete() = 0;
	virtual ~Item() = 0 {};
	// Getters
	int getAttributes() const { return _attributes; }
	int getSize() const { return _size; }
	string getName() const { return _name; }
	// Setters
	void setAttributes(int newAtrribute) { this->_attributes = newAtrribute; }
	void setName(string name) { this->_name = name; }
	void setSize(int newSize) { this->_size = newSize; }
};

// Predicats for Sort

bool sizeS(const Item* it1, const Item* it2) {
	return it1->getSize() < it2->getSize();
}
bool sizeN(const Item* it1, const Item* it2) {
	return it1->getName().substr(it1->getName().rfind("\\") + 1)[0] < it2->getName().substr(it2->getName().rfind("\\") + 1)[0];
}


// File Class

class File:public Item
{
public:

	// Methods
	void Create(string newName) override{
		ofstream file(newName.c_str(),ios::app);
		if (!file) throw BadDirCreate();
		if (!file.is_open()) throw BadDirOpen();
		this->setName(newName);
		this->setAttributes(newName.size());
		file.close();
	}
	void Print() override {
		 Item::Print();
		 cout << "Type: File" << endl;
	};
	void Rename(string newName) override {
		if (rename(getName().c_str(),newName.c_str()) != -1) {
			this->setAttributes(newName.size());
			this->setName(newName);
			return;
		}
		throw BadDirRename();
	};
	void Delete() override {
		if (remove(getName().c_str()) != -1) {
			setName("");
			setSize(0);
			setAttributes(0);
			return;
		}
		throw BadDirRemove();
	}
	void writetoFile(string text)  {
		 ofstream file(getName().c_str(), ios::out);
		 if (!file) throw BadDirCreate();
		 if (!file.is_open()) throw BadDirOpen();
		 file << text<<endl;
		 file.close();
	}
	void appendtoFile(string text) {
		 ofstream file(getName().c_str(), ios::app);
		 if (!file) throw BadDirCreate();
		 if (!file.is_open()) throw BadDirOpen();
		 file << text;
		 file.close();
	}
	void Read() {
		ifstream file(getName().c_str(), ios::in);
		if (!file) throw BadDirCreate();
		if (!file.is_open()) throw BadDirOpen();
		string data;
		while (!file.eof()) {
			getline(file, data);
			cout << data << endl;
		}
		file.close();
	}
};




class Folder:public Item
{
	vector<Item*>folderItems;
public:
	// Getter
	vector<Item*> getFolderItems() { return folderItems; }

	// Methods
	void Create(string newName) override {
		if (_mkdir(newName.c_str()) != -1) {
			this->setName(newName);
			this->setAttributes(newName.size());
			return;
		}
		throw BadDirCreate();
	}
	void Rename(string newName) override  {
		if (rename(getName().c_str(), newName.c_str()) != -1) {
			this->setName(newName);
			this->setAttributes(newName.size());
			return;
		}
		throw BadDirRename();
	}
	void Print() override  {
		Item::Print();
		cout << "Type: Folder" << endl;
		for (auto& i : folderItems)
		{
			cout << endl;
			i->Print();
		}
	}
	void Delete() override {
		if (_rmdir(getName().c_str()) != -1) {
			setName("");
			setAttributes(0);
			folderItems.clear();
			return;
		}
		throw BadDirRemove();
	}
	void CalculateSize()  {
		int total = 0;
		for (auto& i : folderItems)
		{
			total += i->getSize();
		}
		setSize(total);
	}
	int setItems(string location) {
		_finddata_t file_info;
		intptr_t handle = _findfirst((location + "/*").c_str(), &file_info);

		if (handle == -1) {
			return handle;
		}

		this->folderItems.clear();
		
		do {
			if (strcmp(file_info.name, ".") == 0 || strcmp(file_info.name, "..") == 0) {
				continue;
			}

			Item* i1;

			if (file_info.attrib & _A_SUBDIR) {
				i1 = new Folder();
			}
			else {
				i1 = new File();
			}

			string dataName = location + "\\" + string(file_info.name);
			i1->setName(dataName);
			i1->setAttributes(dataName.size());
			i1->setSize(file_info.size);
			this->folderItems.push_back(i1);

		} while (_findnext(handle, &file_info) == 0);
		this->CalculateSize();
		this->setName(location);
		this->setAttributes(location.size());
		_findclose(handle);
		return 0;
	}
	void sort_(bool isSize=true) {
		if(isSize) sort(folderItems.begin(), folderItems.end(), sizeS);
		else sort(folderItems.begin(), folderItems.end(), sizeN);
	}

	// Destructor
	~Folder()
	{
		for (auto& i : folderItems){delete i;}
	}

};



class FileManager
{
	string currentRoot;
	Folder* currentFolder;
public:
	// Default c-tor
	FileManager() : currentFolder(new Folder()),currentRoot("C:\\Users\\Public\\Documents") {
		currentFolder->setItems(currentRoot);
	}
	
	// Getters
	Folder* getCurrentFolder() { return currentFolder; }
	string getCurrentRoot() { return currentRoot; }
	
	// Methods
	void mkdir(string newName) {
		Folder* f = new Folder();
		try {
			f->Create(currentRoot + "\\" + newName);
		}
		catch (const exception& ex) { 
			cout << ex.what() << endl;
			return;
		}
		currentFolder->getFolderItems().push_back(f);
		currentFolder->setItems(currentRoot);
		delete f;

	}
	void rmdir(string deleteName) {
		deleteName = currentRoot + "\\" + deleteName;
		for (auto& i : currentFolder->getFolderItems())
		{
			if (i->getName() == deleteName) {
				try {
					i->Delete();
				}
				catch (const exception& ex) {
					cout << ex.what() << endl;
					return;
				}
				currentFolder->setItems(currentRoot);
				return;
			}
		}
		try { throw BadDirRemove(); }
		catch (const exception& ex) { cout << ex.what() << endl; }
	}
	void type_nul(string newName) {
		File* f = new File();
		try {
			f->Create(currentRoot + "\\" + newName);
		}
		catch (const exception& ex) {
			cout << ex.what() << endl;
			return;
		}
		currentFolder->getFolderItems().push_back(f);
		currentFolder->setItems(currentRoot);
		delete f;
	}
	void rename(string oldName,string newName) {
		newName = currentRoot + "\\" + newName;
		for (auto& i : currentFolder->getFolderItems())
		{
			if (i->getName() == currentRoot + "\\" + oldName) {
				try {
					i->Rename(newName);
				}
				catch (const exception& ex) {
					cout << ex.what() << endl;
					return;
				}
				currentFolder->setItems(currentRoot);
				return;
			}
		}
		try { throw BadDirRename(); }
		catch (const exception& ex) { cout << ex.what() << endl; }
	}
	void cd(string newCurrent) {
		try {
			if (currentFolder->setItems(currentRoot + "\\" + newCurrent) == 0) {
				currentRoot = currentRoot + "\\" + newCurrent;
			}
			else {
				cout << "Dir doesn't exist" << endl;
			}
		}
		catch (const exception& ex) {
			cout << ex.what() << endl;
		}
	}
	void dir_s() {
		currentFolder->Print();
	}
	void cd_backwards() {
		string tempCurrent = currentRoot.substr(0, currentRoot.rfind("\\"));
		try {
			if(currentFolder->setItems(tempCurrent)==0) currentRoot = tempCurrent;
		}
		catch (const exception& ex) {
			cout << ex.what() << endl;
		}
	}
	void cdallback() {
		string tempCurrent = "C:";
		try {
			if (currentFolder->setItems(tempCurrent) == 0) currentRoot = tempCurrent;
		}
		catch (const exception& ex) {
			cout << ex.what() << endl;
		}
	}
	void move(string standerName, string moverName) {
		bool check1 = false;
		for (auto& i : currentFolder->getFolderItems()) {if (i->getName() == currentRoot + "\\" + standerName) check1 = true;}
		if (!check1) {
			try { throw BadDirMove(); }
			catch (const exception& ex) { 
				cout << ex.what() << endl; 
				return;
			}
		};
		rename(moverName, standerName + "\\" + moverName);
	}
	void open(string file) {
		if (file == "") file = currentRoot;
		else file = currentRoot + "\\" + file;
		ShellExecuteA(0, 0, file.c_str(), 0, 0, SW_SHOW);
	}
	void type(string fileName) {
		for (auto& i : currentFolder->getFolderItems())
		{
			if (i->getName() == currentRoot + "\\"+ fileName) {
				if (dynamic_cast<File*>(i) != NULL) {
					try { (dynamic_cast<File*>(i))->Read(); }
					catch (const exception& ex) { cout << ex.what() << endl; }
					return;
				}
			}
		}
		try { throw BadDirOpen(); }
		catch (const exception& ex) { cout << ex.what() << endl;}
	}
	void compare(string name1, string name2) {
		for (auto& i :currentFolder->getFolderItems())
		{
			if (i->getName() == currentRoot + "\\" + name1) {
				for (auto& j : currentFolder->getFolderItems())
				{
					if (j->getName() == currentRoot + "\\" + name2) {
						if (i->getSize() > j->getSize()) cout << name1 << " is larger in size" << endl;
						else if (i->getSize() < j->getSize()) cout << name2 << " is larger in size" << endl;
						else cout << "This files have the same size" << endl;
						return;
					}
				}
			}
		}
		try { throw BadDirOpen(); }
		catch (const exception& ex) { cout << ex.what() << endl; }
	}
	void write(string fileName) {
		for (auto& i : currentFolder->getFolderItems())
		{
			if (i->getName() == currentRoot + "\\" + fileName) {
				if (dynamic_cast<File*>(i) != NULL) {
					string text;
					cout << "Enter text:\n";
					getline(cin, text);
					try { (dynamic_cast<File*>(i))->writetoFile(text); }
					catch (const exception& ex) {cout << ex.what() << endl;}
					currentFolder->setItems(currentRoot);
					return;
				}
			}
		}
		try { throw BadDirOpen(); }
		catch (const exception& ex) { cout << ex.what() << endl; }
	}
	void append(string fileName) {
		for (auto& i : currentFolder->getFolderItems())
		{
			if (i->getName() == currentRoot + "\\" + fileName) {
				if (dynamic_cast<File*>(i) != NULL) {
					cout << "Enter text:\n";
					string text;
					getline(cin, text);
					if (dynamic_cast<File*>(i) != NULL) {
						try { (dynamic_cast<File*>(i))->appendtoFile(text); }
						catch (const exception& ex) {cout << ex.what() << endl;}
						currentFolder->setItems(currentRoot);
						return;
					}
				}
			}
		}
		try { throw BadDirOpen(); }
		catch (const exception& ex) { cout << ex.what() << endl; }
	}
	void sortF(bool isSize=true) {
		currentFolder->sort_(isSize);
	}
	void help() {
		cout << "mkdir --> Creates a new folder" << endl;
		cout << "rmdir --> Deletes selected folder" << endl;
		cout << "cd --> Changes the current directory" << endl;
		cout << "cd.. --> Changes the current directory backwards" << endl;
		cout << "cd/ --> Changes the current directory to C:" << endl;
		cout << "dir --> Displays all content of directories" << endl;
		cout << "open --> Opens selected directory" << endl;
		cout << "type_nul --> Creates a new file" << endl;
		cout << "del/f --> Deletes selected file" << endl;
		cout << "move --> Moves selected directory" << endl;
		cout << "rename --> Renames selected directory" << endl;
		cout << "datetime --> Displays current date and time" << endl;
		cout << "type --> Displays the content of selected file" << endl;
		cout << "shutdown --> Allows proper local or remote shutdown of machine" << endl;
		cout << "compare --> Compares two directories by their size" << endl;
		cout << "write --> Writes text to the selected file" << endl;
		cout << "append --> Appends text to the selected file" << endl;
		cout << "cls --> Clears the terminal screen" << endl;
		cout << "color --> Changes the color of terminal" << endl;
		cout << "exit --> Closes the terminal" << endl;
	}


	// Destructor
	~FileManager() {delete currentFolder;}

};
