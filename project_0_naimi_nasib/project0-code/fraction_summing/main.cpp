#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "fraction_toolbox.hpp"

using namespace std;

// read command line arguments
static void readcmdline(fraction & frac, int argc, char* argv[])
{
    if (argc!=3)
    {
        printf("Usage: n d\n");
        printf("  n        numerator of fraction\n");
        printf("  d        denominator of fraction\n");
        exit(1);
    }

    // read n
    frac.num = atoi(argv[1]);

    // read d
    frac.denom = atoi(argv[2]);
}

static void test23467(int argc, char* argv[])
{
    // TODO: implement function
    fraction frac;
    readcmdline(frac, argc, argv);
    cout << "Input Fraction: " << frac.num << " / " << frac.denom << endl;
    fraction square = square_fraction(frac);
    cout << "Testing function 2\t Result: " << square.num << " / " << square.denom << endl;
    fraction tmp = frac;
    square_fraction_inplace(tmp);
    cout << "Testing function 3\t Result: " << tmp.num << " / " << tmp.denom << endl;
    cout << "Testing function 4\t Result: " << fraction2double(frac) << endl;
    cout << "Testing function 6\t Result: " << gcd(frac) << endl;
    tmp = frac;
    reduce_fraction_inplace(tmp);
    cout << "Testing function 7\t Result: " << tmp.num << " / " << tmp.denom << endl;
}

static void test5()
{
    // TODO: implement function
    int a;
    int b;
    cout << "Enter integer a: ";
    cin >> a;
    cout << "Enter integer b: ";
    cin >> b;
    int divider = gcd(a, b);
    cout << "Testing function 5\t Result: " << divider << endl;
}

static void test_array_functions(int n)
{
    // TODO: implement function
    cout << "Enter length of array: ";
    cin >> n;
    fraction frac_array[n];
    for (int i = 0; i < n; i++) {
        frac_array[i].num = 1;
        frac_array[i].denom = (i+1)*(i+2);
    }
    print_fraction_array(frac_array, n);
    fraction result;
    result = sum_fraction_array(frac_array,n);
    cout << "Testing sum_fraction_array function\t\t Result: " << result.num << " / " << result.denom << endl;

    double result_approx;
    result_approx = sum_fraction_array_approx(frac_array,n);
    cout << "Testing sum_fraction_array_approx function\t Result: " << result_approx << endl;
    // TODO: find n for which sum function breaks. Explain what is happening.
}

static void test_toolbox(int argc, char* argv[])
{
    cout << "\n===============  test23467  =============== " << endl;
    test23467(argc, argv);
    
    cout << "\n=================  test5  ================= " << endl;
    test5();

    cout << "\n==========  test_array_functions  ========= " << endl;
    int n = 5;
    test_array_functions(n);
}

int main(int argc, char* argv[])
{
    test_toolbox(argc, argv);
}