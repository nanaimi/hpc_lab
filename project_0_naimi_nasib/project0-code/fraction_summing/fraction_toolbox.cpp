#include <iostream>

#include "fraction_toolbox.hpp"

void print_fraction(fraction frac)
{
    std::cout << frac.num << '/' << frac.denom << std::endl;
}

void print_fraction_array(fraction frac_array[], int n)
{
    std::cout << "[ " << frac_array[0].num << '/' << frac_array[0].denom << std::endl;
    for (int i = 1; i < n-1; i++)
    {
        std::cout << "  ";
        print_fraction(frac_array[i]);
    }
    std::cout << "  " << frac_array[n-1].num << '/' << frac_array[n-1].denom << " ]" << std::endl;
}

fraction square_fraction(fraction frac)
{
    // TODO: implement function 2
    fraction result;
    result.num = frac.num * frac.num;
    result.denom = frac.denom * frac.denom;
    return result;
}

//TODO: implement function 3
void square_fraction_inplace(fraction & frac)
{
    frac.num = frac.num * frac.num;
    frac.denom = frac.denom * frac.denom;
}

double fraction2double(fraction frac)
{
    // TODO: implement function 4
    double result = double(frac.num) / double(frac.denom);
    return result;
}

int gcd(int a, int b)
{
    // TODO: implement function 5
    if (b == 0) {
        return a;
    } else {
        return gcd(b, a % b);
    }
}

// TODO: implement function 6
int gcd(fraction frac)
{
    int a = frac.num;
    int b = frac.denom;
    int t = 0;
    while (b != 0) {
        t = b;
        b = a % b;
        a = t;
    } 
    return a;
}

void reduce_fraction_inplace(fraction & frac)
{
    // TODO: implement function 7
    int divider = gcd(frac);
    frac.num = frac.num / divider;
    frac.denom = frac.denom / divider;
    // TODO: add short comment to explain which of the gcd() functions your code is calling
    // uses the iterative version
}

fraction add_fractions(fraction frac1, fraction frac2)
{
    //TODO: implement function 8
    fraction result;
    result.num = frac1.num*frac2.denom + frac2.num*frac1.denom;
    result.denom = frac1.denom*frac2.denom;
    reduce_fraction_inplace(result);
    // std::cout << result.num << " / " << result.denom << std::endl;
    return result;
}

double sum_fraction_array_approx(fraction frac_array[], int n)
{
    // TODO: implement function 9
    double result = 0;
    for (int i = 0; i < n; i++) {
        result += fraction2double(*(frac_array+i));
    }
    return result;
    
    // TODO: add short comment to explain why this function is approximate
}

// TODO: implement function 10
fraction sum_fraction_array(fraction frac_array[], int n)
{
    // TODO: implement function 9
    fraction result;
    result.num=0;
    result.denom=1;
    for (int i = 0; i < n; i++) {
        result = add_fractions(result, *(frac_array+i));
    }
    return result;
    
    // TODO: add short comment to explain why this function is approximate
}

void fill_fraction_array(fraction frac_array[], int n)
{
    fraction temp_frac;
    temp_frac.num = 1;
    for (int i = 1; i <= n; i++)
    {
        temp_frac.denom = i * (i+1);
        frac_array[i-1] = temp_frac;
    }
}

