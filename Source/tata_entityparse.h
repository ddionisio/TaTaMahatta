#ifndef _tata_entityparse_h
#define _tata_entityparse_h

class EntityParse {	//this is used for loading entities
public:
	EntityParse() {}
	~EntityParse() {}

	//add variables from given buff
	void Parse(char *buff)
	{
		char lVal[MAXCHARBUFF], rVal[MAXCHARBUFF];
		char **walker = &buff;

		while(**walker != 0)
		{
			//get the two string for lVal and rVal
			ParserReadStringBuff(walker, lVal, MAXCHARBUFF, '"', '"');
			ParserReadStringBuff(walker, rVal, MAXCHARBUFF, '"', '"');
			
			//skip newline
			ParserSkipCharBuff(walker, '\n');

			AddVar(lVal, rVal);
		}
	}

	//add a variable with given value
	void AddVar(const char *lVal, const char *rVal)
	{
		if(GetVal(lVal))
			return;

		EntVar newVar;
		newVar.lVal = lVal;
		newVar.rVal = rVal;

		m_vars.push_back(newVar);
	}

	//get value of variable (0 if not found)
	const char *GetVal(const char *varName) const
	{
		for(int i = 0; i < m_vars.size(); i++)
		{
			if(stricmp(m_vars[i].lVal.c_str(), varName) == 0)
				return m_vars[i].rVal.c_str();
		}

		return 0;
	}

private:
	struct EntVar {
		string lVal;	//variable
		string rVal;	//value
	};

	vector<EntVar> m_vars;	//the variables
};

#endif