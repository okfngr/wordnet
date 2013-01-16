#include "pugixml.hpp"
#include <iostream>
#include <fstream>
#include <ios>
#include <sstream>
#include <string>
#include <cmath>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>
#include <utility>

using namespace std;

string StringClean(string the_string){
	char bad[] = {'.', ',', ' ',')','(','\'','\"'};
	string::size_type n = 0;

	while ((n=the_string.find(bad[0], n))!=the_string.npos){
		the_string.replace(n,1,"_");
	}

	n = 0;
	while ((n=the_string.find(bad[1], n))!=the_string.npos){
		the_string.replace(n,1,"_");
	}

	n = 0;
	while ((n=the_string.find(bad[2], n))!=the_string.npos){
		the_string.replace(n,1,"_");
	}
	
	n = 0;
	while ((n=the_string.find(bad[3], n))!=the_string.npos){
		the_string.erase(n,1);
	}
	n = 0;
	while ((n=the_string.find(bad[4], n))!=the_string.npos){
		the_string.erase(n,1);
	}
	
	n = 0;
	while ((n=the_string.find(bad[5], n))!=the_string.npos){
		the_string.erase(n,1);
	}
	
	n = 0;
	while ((n=the_string.find(bad[6], n))!=the_string.npos){
		the_string.erase(n,1);
	}
	return the_string;
}
string StringClean2(string the_string){
	char bad[] = {'"'};
	string::size_type n = 0;

	while ((n=the_string.find(bad[0], n))!=the_string.npos){
		the_string.replace(n,1,"\\\"");
		n=n+3;
	}
	return the_string;
}

string URIFinder (string id){
	ifstream index;
	index.open("data/index.txt");
	bool check = false;
	string id1,uri,uri_temp;
	uri="NULL";
	while (!check && !index.eof()){
		index >> id1 >> uri_temp;
		if (id1.compare(id)==0){
			check=true;
			uri=uri_temp;
			}
		}
	return uri;
}

string Mapper (string field){
	ifstream mappings;
	mappings.open("initialization/mappings.txt");
	bool check = false;
	string field1,mapping,mapping_temp;
	mapping="NULL";
	while (!check && !mappings.eof()){
		mappings >> field1 >> mapping_temp;
		if (field1.compare(field)==0){
			check=true;
			mapping=mapping_temp;
			}
		}
	return mapping;
}

string PartOfSpeech(string pos){
	string partof;
	if(pos=="n"){
		partof="noun";
		}
	else if(pos=="v"){
		partof="verb";
		}
	else if(pos=="b"){
		partof="adverb";
		}
	else if(pos=="a"){
		partof="adjective";
		}
	return partof;
	}
string WordSense(string pos){
	string partof;
	if(pos=="n"){
		partof="NounWordSense";
		}
	else if(pos=="v"){
		partof="VerbWordSense";
		}
	else if(pos=="b"){
		partof="AdverbWordSense";
		}
	else if(pos=="a"){
		partof="AdjectiveWordSense";
		}
	return partof;
	}


int main()
{
	/////////////////////////////////////////////////////////DECLARATIONS////////////////////////////////////////////////////////////
	pugi::xml_document doc;
	int k=0;
	int j=0;
	int x=0;
	// get a test document
    doc.load_file("wngre2.xml",pugi::encoding_auto);
	ofstream ilrm1;
	ilrm1.open("data/ilrm1.txt");
	ofstream index;
	index.open("data/index.txt");
	ofstream sid1;
	sid1.open("data/synsetid.n3");
	
	
	////////////////////////////////////////////////////////LOADING MAPPINGS/////////////////////////////////////////////////////////
	//reading of the mappings
	ifstream mappings;
	
	mappings.open("initialization/mappings.txt");
	int z=0;
	int max_map;
	int invalid_mappings=0;
	string map[100][2];
	cout << "Loading mappings..."<<endl;
	if (mappings.is_open()) {
		while(!mappings.eof()){
			mappings >> map[z][0]>>map[z][1];
			if(map[z][1]!=""){
				cout << map[z][0]<<"--------"<<map[z][1]<<endl;
				z++;
			}
			else {
				invalid_mappings++;
				cout << "WARNING!!!!!!Mapping with field tag "<<map[z][0]<<" is empty!!!Please check the file mappings.txt"<<endl;
			}
			
		}
		max_map=z;
		mappings.close();
		cout<< "Valid mappings #: "<<max_map<<"                               "<<"Invalid mappings #: "<< invalid_mappings<<endl<<endl<<endl;
	}
	else {
		cout<< "The file mappings.txt does not exist. Make sure there is a proper syntaxed file named mappings.txt in the folder";
		return 0;
	}
	ofstream rdf;
	rdf.open("data/wordnet-synsets.n3");
	
	
	/////////////////////////////////////////////////////////LOADING PREFIXES/////////////////////////////////////////////////////////
	//reading of prefixes
	ifstream prefixes;
	prefixes.open("initialization/prefixes.txt");
	z=0;
	int max_prefix;
	string prefix[50];
	cout<< "Loading prefixes..."<<endl;
	if (prefixes.is_open()) {
		while(!prefixes.eof()){
			getline (prefixes,prefix[z]);
			cout << prefix[z]<<endl;
			rdf <<prefix[z]<<endl;
			z++;
		}
		max_prefix=z;
		cout << max_prefix<<endl;
	}
	else {
		cout<< "The file prefixes.txt does not exist. Make sure there is a proper syntaxed file named prefixes.txt in the folder";
		return 0;
	}
	
////////////////////////////////////////////////////LOADING XML///////////////////////////////////////////////////////////////////////////    
	pugi::xml_node wordnet = doc.child("WordNet");
    //reading file
    for (pugi::xml_node synset= wordnet.first_child(); synset; synset=synset.next_sibling("SYNSET")){
		string uri,id,pos,def,stamp,usage,bcs,synonymm[1000][3],ilrm[1000][2];
		stringstream ss;
		id = synset.child("ID").text().get();
		pos = synset.child("POS").text().get();
		k=0;
		for (pugi::xml_node_iterator it = synset.child("SYNONYM").begin(); it != synset.child("SYNONYM").end(); ++it){
			synonymm[k][0] = it->child_value();
			synonymm[k][1] = it->child_value("SENSE");
			synonymm[k][2] = it->child_value("LNOTE");
			k++;
		}
		def = synset.child("DEF").text().get() ;
		stamp = synset.child("STAMP").text().get() ;
		usage = synset.child("USAGE").text().get() ;
		bcs = synset.child("BCS").text().get() ;
		
		for (pugi::xml_node ilr = synset.child("ILR"); ilr; ilr = ilr.next_sibling("ILR")){
			ilrm1<<id<<'\t'<<ilr.text().get()<<'\t'<<ilr.child("TYPE").text().get()<<endl;
			x++;
		}
		j++;
		//////////////////////////////////////////////MAKE THE URI/////////////////////////////////////////////////////////////////////////
		string partof;
		partof=PartOfSpeech(pos);
		cout<<"-----"<<j<<"--------\n";
		ss<<"wngre:synset-"<<synonymm[0][0]<<"-"<<partof<<"-"<<synonymm[0][1];
		uri=ss.str();
		uri=StringClean(uri);
		cout<<uri<<"\n";
		index<<id<<'\t'<<uri<<endl;
	
		//////////////////////////////////////////////MAKE THE TRIPLES/////////////////////////////////////////////////////////////////////
	
		rdf<<uri<<'\t'<<"rdfs:label"<<'\t'<<'"'<<StringClean2(synonymm[0][0])<<'"'<<"@el ."<<"\n";
	
		string pred;
		pred=Mapper("DEF");
		if(pred.compare("NULL")!=0){
			rdf<<uri<<'\t'<<pred<<'\t'<<'"'<<StringClean2(def)<<'"'<<"@el ."<<"\n";
		}
		pred=Mapper("BCS");
		if(pred.compare("NULL")!=0){
			rdf<<uri<<'\t'<<pred<<'\t'<<'"'<<bcs<<'"'<<" ."<<"\n";
		}
		
		pred=Mapper("ID");
		if(pred.compare("NULL")!=0){
			rdf<<uri<<'\t'<<pred<<'\t'<<'"'<<id<<'"'<<" ."<<"\n";
			string str1,str2,str3;
			int pos1,pos2,sid;
			pos1=id.find_first_of("-");
			pos2=id.find_last_of("-");
			if(pos1!=5){
				continue;
			}
			if(pos2!=14){
				continue;
			}
			str1=id.substr(0,pos1);
			cout<<str1<<"\n";
			if(str1.compare("ENG20")!=0){
				continue;
			}
			str2=id.substr(pos1+1,8);
			str3=id.substr(pos2+1);
			if(str3.compare("n")==0){
				str2.insert(0,"1");
			}
			else if(str3.compare("v")==0){
				str2.insert(0,"2");
			}
			else if(str3.compare("a")==0){
				str2.insert(0,"3");
			}
			else if(str3.compare("b")==0){
				str2.insert(0,"4");
			}
			sid1<<uri<<'\t'<<"rdf:id"<<'\t'<<'"'<<str2<<'"'<<" .\n";
		}

		pred=Mapper(pos);
		if(pred.compare("NULL")!=0){
			rdf<<uri<<'\t'<<"rdf:type"<<'\t'<<pred<<" ."<<"\n";
		}
	/////////////////////////////////////////////MAKE WORDSENSE TRIPLES/////////////////////////////////////////////////////////////////
		string ws;
		ws = WordSense(pos);
		for (int i=0;i<k;i++){
			string wordsense;
			ss.str("");
			ss<<"wngre:wordsense-"<<synonymm[i][0]<<"-"<<partof<<"-"<<synonymm[i][1];
			wordsense=ss.str();
			wordsense=StringClean(wordsense);
			rdf<<uri<<'\t'<<"wn20s:containsWordSense"<<'\t'<<wordsense<<" ."<<"\n";
			rdf<<wordsense<<'\t'<<"rdf:type"<<'\t'<<"wn20s:"<<ws<<" .\n";
			rdf<<wordsense<<'\t'<<"rdfs:label"<<'\t'<<'"'<<StringClean2(synonymm[i][0])<<'"'<<"@el .\n";
			rdf<<wordsense<<'\t'<<"wngre-onto:lnote"<<'\t'<<'"'<<StringClean2(synonymm[i][2])<<'"'<<"@en .\n";
			rdf<<wordsense<<'\t'<<"wn20s:sense"<<'\t'<<'"'<<StringClean2(synonymm[i][1])<<'"'<<" .\n";
			rdf<<wordsense<<'\t'<<"wn20s:word"<<'\t'<<"wngre:word-"<<StringClean(synonymm[i][0])<<" .\n";
			rdf<<"wngre:word-"<<StringClean(synonymm[i][0])<<'\t'<<"wn20s:lexicalForm"<<'\t'<<'"'<<StringClean2(synonymm[i][0])<<'"'<<"@el .\n";
			rdf<<"wngre:word-"<<StringClean(synonymm[i][0])<<'\t'<<"rdfs:label"<<'\t'<<'"'<<StringClean2(synonymm[i][0])<<'"'<<"@el .\n";
			rdf<<"wngre:word-"<<StringClean(synonymm[i][0])<<'\t'<<"rdf:type"<<'\t'<<"wn20s:Word .\n";
			rdf<<"wngre:word-"<<StringClean(synonymm[i][0])<<'\t'<<"wnge-onto:lnote"<<'\t'<<'"'<<StringClean2(synonymm[i][0])<<'"'<<".\n";
		}
	
	}
	/////////////////////////////////////////////MAKE ILR TRIPLES///////////////////////////////////////////////////////////////////////
    ilrm1.close();
	index.close();
	ifstream ilrm2;
	ilrm2.open("data/ilrm1.txt");
	string prev1,prev2,uri1,uri2;
	prev1="";
	prev2="";
	k=0;
	while(!ilrm2.eof()){
		string id1,id2,type1;
		ilrm2 >> id1 >> id2 >> type1;
		string pred;
		pred=Mapper(type1);
		
		if(pred.compare("NULL")!=0){
			if(prev1.compare(id1)!=0){
				uri1=URIFinder(id1);
				prev1=id1;
			}
			if(prev2.compare(id2)!=0){			
				uri2=URIFinder(id2);
				prev2=id2;
			}
			if(uri1.compare("NULL")!=0 &&	uri2.compare("NULL")!=0){
				rdf<<uri1<<'\t'<<pred<<'\t'<<uri2<<" ."<<"\n";
			}
		}
		k++;
		double perc;
		perc=(double(k)/double(x))*100;
		cout << "\rProgress of ILR conversion: " << perc << "%" << flush;
		//cout << id1 <<"       "<<id2 <<"       "<<type1<<endl;
		//cout << URIFinder(id1) <<"       "<<URIFinder(id2) <<"       "<<type1<<endl;
		
	}
	rdf.close();
	ilrm2.close();
	sid1.close();
   
}
