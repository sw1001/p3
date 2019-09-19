/* 
 * File:   C_setInfluence_wcz.cl
 * Author: chenyuan
 *
 * Created on July 22, 2019
 */

#define N 100 // N > index, which is the total number of literals

__kernel void setInfluence(const int deviceIndex, const int literal, const int literals, const int size, const int dim1_size, __global int* lambdas, __global float* lambdap, __global int* dim2_size, __global float* parameters, __global int* resultOnce) {   
	int id = deviceIndex * get_global_size(0) + get_global_id(0);
	int assignment[N];
    
    //or try to get newlambda like original version does	
	for(int j = 0; j < size; j++) {
	    float rand = parameters[id * literals + lambdas[j]];
		if(lambdap[j] < rand)
			assignment[lambdas[j]] = 0;
		else
			assignment[lambdas[j]] = 1;				
	}
    //the true case
    assignment[literal] = 1;
    int valuet = 0;
    int index = 0;
    for(int m = 0; m < dim1_size; m++) {
    	int valueMono = 1;
    	for(int n = 0; n < dim2_size[m]; n++) {
        	if(assignment[lambdas[index + n]] == 0) {
        		valueMono = 0;
        		index += dim2_size[m];
        		break;
        	}
    	}
    	if(valueMono == 1) {
        	valuet = 1;
        	break;
    	}
    }        
    //the false case
    assignment[literal] = 0;
    int valuef = 0;
    index = 0;
    for(int m = 0; m < dim1_size; m++) {
    	int valueMono = 1;
    	for(int n = 0; n < dim2_size[m]; n++) {
        	if(assignment[lambdas[index + n]] == 0) {
        		valueMono = 0;
        		index += dim2_size[m];
        		break;
        	}
    	}
    	if(valueMono == 1) {
        	valuef = 1;
        	break;
    	}
    }            
    
	resultOnce[id] = valuet - valuef;
	//printf("id=%d->value=%d ", id, resultOnce[id]);
	
}  


