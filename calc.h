
/*
    Class that holds a number which is represented internally as a sequence of integers.
    Its methods include initialization with int\string\list, copying, simplifying to legal representation, and conversion to a string. 
    The digit in the number can be indexed;
    Arithmetic calculation for non-negative integers are implemented with operator overload.
*/
#include <vector>
#include <string>
#include <iterator>
#include <cmath>
#include <iomanip>
#include <stdexcept>
#include <iostream>
#include <sstream>

template<int BASE=100>
class Num
{
private:
    std::vector<int> num;

public:
    Num()
    {}

    Num(const std::string & st)  // init with string
    {
        // split string into 2-character sequence in reverse order
        int bl = std::log10(BASE);
        int l = st.length() / bl;
        this->num = std::vector<int>(l);

        auto it = std::begin(this->num);   //may not compile
        //std::vector<int>::iterator it = this->num.begin();
        int i;
        for(i=st.length(); i>bl; i-= bl, it++)
            *it = std::stoi(st.substr(i-bl,bl));
        if(i == bl)
            *it = std::stoi(st.substr(0,bl));
        else
            this->num.push_back(std::stoi(st.substr(0,i)));

        this->simplify();  // derive a legal representation
    }

    Num(const std::vector<int> & l): num(l)  //init directly with an array, no simplification
    {}

    Num(int i)                      //init with an integer
    {
        this->num = std::vector<int>(1, i);
        this->simplify();  // derive a legal representation
    }

    //use default copy & move constructor & assignment

    int& at(int i)                      //lvalue index, allow negative index
    {
        if(i>=0 && i<this->num.size())
            return this->num[i];
        else if(i<0 && -i <= this->num.size())
            return this->num[this->num.size()+i];
        else if (i == this->num.size())  //allow extension
        {
            this->num.push_back(0);
            return this->num[i];
        }
        else
            throw std::out_of_range ("lvalue index out_of_range.");
    }

    int at(int i) const           //rvalue index, allow any index
    {
        if(i>=0 && i<this->num.size())
            return this->num[i];
        else if(i<0 && -i <= this->num.size())
            return this->num[this->num.size()+i];
        else
            return 0;
    }

    int& operator[](int i) 
    {
        return this->at(i);
    }

    int operator[](int i) const
    {
        return this->at(i);
    }

    Num<BASE> slice(int m=0, int n=0) const
    {
        std::vector<int>::const_iterator itm, itn;
        if(m>=0 && m <= this->num.size())
            itm = this->num.cbegin() + m;
        else if (m<0 && -m <= this->num.size())
            itm = this->num.cend() + m;
        else
            throw std::out_of_range ("slice out_of_range. m = " + std::to_string(m));

        if(n>0 && n <= this->num.size())
            itn = this->num.cbegin() + n;
        else if (n<=0 && -n <= this->num.size())
            itn = this->num.cend() + n;
        else
            throw std::out_of_range ("slice out_of_range. n = " + std::to_string(n));

        return std::vector<int>(itm, itn);
    }
    
    Num<BASE> prepend( const Num<BASE> & b) const
    {
        std::vector<int> v(this->num.begin(), this->num.end());
        v.insert(v.end(), b.num.cbegin(), b.num.cend());
        return v;  //RVO
    }

    Num<BASE> simplify()
    {
        //derive a legal representation
        if(this->num.empty()) return *this;  // does not simplify empty object
        // legal integer range
        int i = 0;
        while(i < this->num.size()){
            if (this->at(i)>= BASE){
                this->at(i+1) += this->at(i) / BASE; 
                this->at(i) %= BASE;
            }
            else if(this->at(i)<0){  //cpp's / rounds towards zero
                this->at(i+1) += this->at(i) / BASE - 1;
                this->at(i) = this->at(i) % BASE + BASE;
            }
            i += 1;
        }
        // delete prepending zeros
        while(this->size()>0 && this->at(-1) == 0) this->num.pop_back();
        
        return *this;
    }
            
    std::string str() const
    {
        // representation in string format, delete 0s at the front
        std::stringstream strs;
        std::vector<int>::const_reverse_iterator it = this->num.crbegin();
        while(it != this->num.crend() && *it == 0) it++;
        if(it != this->num.crend()){
            strs<<*it;
            it++;
        }
        else
            strs<<0;
        for (; it != this->num.crend(); it++)
            strs<<std::setw(std::log10(BASE)) << std::setfill('0') << *it;
        return strs.str();
    }

    friend std::ostream & operator<<(std::ostream &os, const Num<BASE> & obj)
    {
        os << obj.str();
        return os;
    }

    friend std::istream & operator>>(std::istream & is, Num<BASE> & obj)
    {
        std::string st;
        is >> st;
        obj = Num<BASE>(st);
        if( 0 /* T could not be constructed */ )
            is.setstate(std::ios::failbit);
        return is;
    }
        
    int size() const
    {
        return this->num.size();
    }

    int isGreaterThan(const Num<BASE> & b) const
        //use legal representation of a & b, otherwise it may give incorrect results
    {
        if(this->size()> b.size()) 
            return 1;
        else if(this->size()< b.size())
            return -1;
        else
        {
            std::vector<int>::const_reverse_iterator  
                ita = this->num.crbegin(),
                itb = b.num.crbegin();
            for(; ita != this->num.rend(); ita++, itb++)
                if(*ita > *itb)
                    return 1;
                else if(*ita < *itb)
                    return -1;
            return 0;
        }
    }

    int isSmallerThan(const Num<BASE> & b) const
    {
        return - this->isGreaterThan(b);
    }

    friend bool operator>(const Num<BASE> & a, const Num<BASE> & b) 
    {
        return a.isGreaterThan(b)==1;
    }

    friend bool operator<(const Num<BASE> & a, const Num<BASE> & b) 
    {
        return b > a;
    }

    friend bool operator>=(const Num<BASE> & a, const Num<BASE> & b) 
    {
        return !(a<b);
    }

    friend bool operator<=(const Num<BASE> & a, const Num<BASE> & b) 
    {
        return !(a>b);
    }

    friend bool operator==(const Num<BASE> & a, const Num<BASE> & b) 
    {
        return a.isGreaterThan(b) == 0;
    }

    friend bool operator!=(const Num<BASE> & a, const Num<BASE> & b) 
    {
        return !(a==b);
    }

    friend Num<BASE> operator+(const Num<BASE> & a, const Num<BASE> & b) 
    {
        // utilize Num<BASE> properties of indexing
        int l = a.size() > b.size() ? a.size() : b.size();
        Num<BASE> t(std::vector<int>(l, 0));
        for(int i=0; i<l; i++){
            t[i] += (a[i] + b[i]);
            t[i+1] += t[i] / BASE; 
            t[i] %= BASE;
        }
        return t;
    }
        
    friend Num<BASE> operator-(const Num<BASE> & a, const Num<BASE> & b) 
    {
        if(a < b) 
            return 0;
        else
        {
            int l = a.size() > b.size() ? a.size() : b.size();
            Num<BASE> t(std::vector<int>(l, 0));

            for(int i=0; i<l; i++){
                t[i] += (a[i] - b[i]);  
                if(t[i] >= BASE){              //c++'s division round towards zero'
                    t[i+1] += t[i] / BASE; 
                    t[i] %= BASE;
                }
                else if(t[i] < 0){
                    t[i+1] += t[i] / BASE - 1;
                    t[i] = t[i] % BASE + BASE; 
                }
            }

            t.simplify();
            return t;  
        }
    }

    friend Num<BASE> operator*(const Num<BASE> & a, const Num<BASE> & b) 
    {
        Num<BASE> t(std::vector<int>(a.size()+b.size(), 0));
        for(int i=0; i<a.size(); ++i)
            for(int j=0; j<b.size(); ++j){
                t[i+j] += a[i] * b[j];
                t[i+j+1] += t[i+j] / BASE;
                t[i+j] %= BASE;
            }
        t.simplify();  // very important
        return t;
    }

    friend Num<BASE> operator/(Num<BASE> a, const Num<BASE> & b) //a need to be assigned in the algorithm, pass by value
    {
        //use legal representation of a, b, otherwise algorithm can fall
        Num<BASE> t(std::vector<int>(a.size(), 0));
        while(a >= b)
        {
            // first slice a to get the part larger than b, which is stored in a1
            // store the reminder part in a2
            // then get the quotient starting with a guess, using two digists of b
            // note that Num class allows indexing that goes beyond length of numbers
            Num<BASE> a1, a2, r; 
            int guess;
            if (a.slice(-b.size()) >= b){
                a1 = a.slice(-b.size());
                a2 = a.slice(0,-b.size());
                guess = (a1[-1]*BASE + a1[-2]) / (b[-1]*BASE + b[-2]);
            }
            else{
                a1 = a.slice(-b.size()-1);
                a2 = a.slice(0,-b.size()-1);
                guess = (a1[-1]* BASE * BASE + a1[-2] * BASE + a1[-3]) / (b[-1]*BASE + b[-2]);
            }
            // increase guess until too large
            while(a1 >= b * guess) ++guess;  
            // decrease guess to a proper value
            while(a1 <  b * guess) --guess;  //cannot use a1 - b*guess < 0 since only non-negative operation is allowed

            t[a2.size()] = guess;
            r = a1 - b * guess;     
            a = a2.prepend(r).simplify();   // must simplify to get legal a & b
        }
        t.simplify();
        return t;
    }

    friend Num<BASE> operator%(const Num<BASE> & a, const Num<BASE> & b) 
    {
        return a - a / b * b;
    }

    friend Num<BASE> sharp(const Num<BASE> & a, const Num<BASE> & b, int m)
    {
        const Num<BASE> & t = a * b;
        return std::vector<int>(t.num.cbegin()+m, t.num.cend()); 
    }
    
}; // class Num

