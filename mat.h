#ifndef __mat__
#define __mat__

#include <cmath>
#include <iostream>

template<class T, int m, int n> struct mat;
template<class T, int m, int n> T dot(const mat<T,m,n>& u,const mat<T,m,n>& v);

template<class T, int m, int n>
struct mat
{
    T x[m][n];

    mat()
    {make_zero();}

    void make_zero()
    {for(int i = 0; i < m; i++) for(int j = 0; j < n; j++) x[i][j] = 0;}

    void make_id()
    {assert(m==n);make_zero();for(int i = 0; i < m; i++) x[i][i] = 1;}

    mat& operator += (const mat& v)
    {for(int i = 0; i < m; i++) for(int j = 0; j < n; j++) x[i][j] += v.x[i][j]; return *this;}

    mat& operator -= (const mat& v)
    {for(int i = 0; i < m; i++) for(int j = 0; j < n; j++) x[i][j] -= v.x[i][j]; return *this;}

    mat& operator *= (const T& c)
    {for(int i = 0; i < m; i++) for(int j = 0; j < n; j++) x[i][j] *= c; return *this;}

    mat& operator /= (const T& c)
    {for(int i = 0; i < m; i++) for(int j = 0; j < n; j++) x[i][j] /= c; return *this;}

    mat operator + () const
    {return *this;}

    mat operator - () const
    {mat r; for(int i = 0; i < m; i++) for(int j = 0; j < n; j++) r[i] = -x[i][j]; return r;}

    mat operator + (const mat& v) const
    {mat r; for(int i = 0; i < m; i++) for(int j = 0; j < n; j++) r[i] = x[i][j] + v.x[i][j]; return r;}

    mat operator - (const mat& v) const
    {mat r; for(int i = 0; i < m; i++) for(int j = 0; j < n; j++) r[i] = x[i][j] - v.x[i][j]; return r;}

    mat operator * (const T& c) const
    {mat r; for(int i = 0; i < m; i++) for(int j = 0; j < n; j++) r[i] = x[i][j] * c; return r;}

    mat operator / (const T& c) const
    {mat r; for(int i = 0; i < m; i++) for(int j = 0; j < n; j++) r[i] = x[i][j] / c; return r;}

    template<int p>
    mat<T,m,p> operator * (const mat<T,n,p>& M) const
    {
        mat<T,m,p> r;
        for(int i = 0; i < m; i++)
            for(int j = 0; j < n; j++)
                for(int k = 0; k < p; k++)
                    r.x[i][k] += x[i][j] * M.x[j][k];
        return r;
    }

    const T& operator() (int i, int j) const
    {return x[i][j];}

    T& operator() (int i, int j)
    {return x[i][j];}
};

template <class T, int m, int n>
mat<T,m,n> operator * (const T& c, const mat<T,m,n>& v)
{return v*c;}

template <class T, int m, int n>
std::ostream& operator << (std::ostream& out, const mat<T,m,n> & M)
{
    for(int i = 0; i < m; i++)
    {
        if(i) out << " ; ";
        for(int j = 0; j < n; j++)
        {
            if(j) out << ' ';
            out << M.x[i][j];
        }
    }
    return out;
}

typedef mat<float,4,4> mat4;

// 4x4 rotation matrix taking vector "from" into vector "to"
//
inline mat4 from_rotated_vector(const vec3& from,const vec3& to)
{
    vec3 A[3]={from.normalized()};
    vec3 B[3]={to.normalized()};
    A[1]=B[1]=cross(A[0],B[0]).normalized();
    A[2]=cross(A[0],A[1]);
    B[2]=cross(B[0],B[1]);

    mat4 M;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            for(int k=0;k<3;k++)
                M(i,j)+=B[k][i]*A[k][j];
    M(3,3)=1;
    return M;
}

#endif
