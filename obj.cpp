#include "obj.h"

#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

obj::material::material()
{
    memset(ka, 0, 3*sizeof(float));
    memset(kd, 0, 3*sizeof(float));
    memset(ks, 0, 3*sizeof(float));
    ns = 0;
    illum = 0;
}

obj::obj()
{
}

obj::~obj()
{
}

bool obj::load_material(const string& filename)
{
    ifstream istr(filename.c_str());
    if (!istr) return false;

    string line, tok;
    string sval;

    string matact;
    while (getline(istr, line))
    {
        stringstream ss(line);
        ss >> sval;

        if (sval == "newmtl")
        {
            ss >> matact;
            mats.insert(
                std::pair<std::string,material>(matact, material())
                 );
        }
        else if (sval == "Ka")
        {
            for (int i = 0; i < 3; ++i)
                ss >> mats[matact].ka[i];
        }
        else if (sval == "Kd")
        {
            for (int i = 0; i < 3; ++i)
                ss >> mats[matact].kd[i];
        }
        else if (sval == "Ks")
        {
            for (int i = 0; i < 3; ++i)
                ss >> mats[matact].ks[i];
        }
        else if (sval == "Ns")
        {
            ss >> mats[matact].ns;
        }
        else if (sval == "illum")
        {
            ss >> mats[matact].illum;
        }
        else if (sval == "map_Ka")
        {
            ss >> mats[matact].map_ka;
        }
        else if (sval == "map_Kd")
        {
            ss >> mats[matact].map_kd;
        }
        else if (sval == "map_Ks")
        {
            ss >> mats[matact].map_ks;
        }
    }

    istr.close();
    return true;
}

bool obj::load(const std::string& filename)
{
    ifstream istr(filename.c_str());
    if (!istr) return false;

    string line, tok;
    int ival;
    float fval;
    char cval;
    string sval;

    string matact("none");

    while (getline(istr, line))
    {
        stringstream ss;
        ss << line;

        ss >> tok;
        if (tok == "v") {
            for (int i = 0; i < 3; ++i)
            {
                ss >> fval;
                verts.push_back(fval);
            }
        }
        else if (tok == "vn")
        {
            for (int i = 0; i < 3; ++i)
            {
                ss >> fval;
                norms.push_back(fval);
            }
        }
        else if (tok == "vt")
        {
            for (int i = 0; i < 2; ++i)
            {
                ss >> fval;
                tex.push_back(fval);
            }
        }
        else if (tok == "f")
        {
            string ftok;
            face f;
            while (ss >> ftok) {
                stringstream ss2;

                ss2 << ftok;
                //cout << ftok << endl;

                ss2 >> ival;
                f.vind.push_back(ival-1);
                //cout << ival << endl;

                if (ss2 && ss2.peek() == '/')
                {
                    ss2 >> cval;

                    // texture coordinates
                    if (ss2.peek() != '/')
                    {
                        ss2 >> ival;
                        f.tex.push_back(ival-1);
                        //cout << fval << endl;
                    }
                }

                // normal indices
                if (ss2 && ss2.peek() == '/')
                {
                    ss2 >> cval;
                    ss2 >> ival;
                    f.nind.push_back(ival-1);
                    //cout << ival << endl;
                }

            };

            //cout << matact << endl;
            f.mat = matact;
            faces.push_back(f);
        }
        else if(tok == "mtllib")
        {
            ss >> sval;
            load_material(sval);
        }
        else if (tok == "usemtl")
        {
            ss >> matact;
            //cout << matact<< endl;
        }
    }

    istr.close();
    return true;
}
