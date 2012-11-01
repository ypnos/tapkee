/*
 *
 *
 * Copyright (c) 2012, Sergey Lisitsyn
 * All rights reserved.
 */

#include "edrt.hpp"
#include "utils/time.hpp"

#include <shogun/features/DenseFeatures.h>
#include <shogun/kernel/LinearKernel.h>
#include <shogun/converter/KernelLocallyLinearEmbedding.h>
#include <algorithm>
#include <string>
#include <istream>
#include <fstream>
#include <vector>
#include <iterator>

using namespace Eigen;
using namespace shogun;
using namespace std;

struct kernel_callback
{
	kernel_callback(CKernel* kernel) : _kernel(kernel) {};
	double operator()(int a, int b) const
	{
		return _kernel->kernel(a, b);
	}
	CKernel* _kernel;
};

vector< vector<double> > read_data(const string& filename)
{
	ifstream ifs(filename.c_str());
	string str;
	vector< vector<double> > input_data;
	while (!ifs.eof())
	{
		getline(ifs,str);
		if (str.size())
		{
			stringstream strstr(str);
			istream_iterator<double> it(strstr);
			istream_iterator<double> end;
			vector<double> row(it, end);
			input_data.push_back(row);
		}
	}
	return input_data;
}

int main(int argc, const char** argv)
{
	if (argc!=2)
	{
		printf("No parameters specified.\n");
		exit(EXIT_FAILURE);
	}

	// Load data
	vector< vector<double> > input_data = read_data("input.dat");

	// Shogun part
	init_shogun_with_defaults();
	SGMatrix<double> fm(input_data[0].size(),input_data.size());
	vector<int> data_indices;
	for (int i=0; i<fm.num_rows; i++)
	{
		for (int j=0; j<fm.num_cols; j++)
			fm(i,j) = input_data[j][i];
	}
	for (int i=0; i<fm.num_cols; i++)
		data_indices.push_back(i);
	CDenseFeatures<double>* features = new CDenseFeatures<double>(fm);
	features = new CDenseFeatures<double>(fm);
	CKernel* kernel = new CLinearKernel(features,features);
	kernel_callback cb(kernel);

	// Embed
	edrt_options_t options;
	DenseMatrix embedding = embed(data_indices.begin(),data_indices.end(),cb,options,2,0,atoi(argv[1]));

	// Save obtained data
	ofstream ofs("output.dat");
	ofs << embedding;
	ofs.close();
	return 0;
}
