#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]){
	ifstream in1(argv[0]);
	ifstream in2(argv[1]);

	vector <string> v1;
	vector <string> v2;

	while(!in1.eof()){
		string temp1;
		getline(cin, temp1);
		v1.push_back(temp1);
	}

	while(!in2.eof()){
		string temp2;
		getline(cin,tmp2);
		v2.push_back(temp2);
	}

	int dif=0;
	if(temp1.size() != temp2.size())
		cout<<"The two files are different."<<endl;
	else{
		for(int i=0; i<temp1.size(); i++){
			if(temp1[i]!=temp2[i])
				dif=1;
		}
	}

	if(dif==0)
		cout<<"The two files are same."<<endl;
	else 
		cout<<"The two files are different."<<endl;
}