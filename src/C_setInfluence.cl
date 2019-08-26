/* 
 * File:   C_setInfluence.cl
 * Author: chenyuan
 *
 * Created on July 22, 2019
 */


#define N 70 // N > index, which is the total number of literals
#define BASE 4294967296UL

uint MWC64X(uint2 *state)
{
    enum { A=4294883355U};
    uint x=(*state).x, c=(*state).y;  // Unpack the state
    uint res=x^c;                     // Calculate the result
    uint hi=mul_hi(x,A);              // Step the RNG
    x=x*A+c;
    c=hi+(x<c);
    *state=(uint2)(x,c)               // Pack the state back up
    return res;                       // Return the next result
}


__kernel void setInfluence(const int literals, const int size, const int dim1_size, __global int* lambdas, __global float* lambdap, __global int* dim2_size, __global float* influence){   
	int flag=get_global_id(0);
	int sum=0;
	int count=10000;
	int assignment[N];
    //or try to get newlambda like original version does
    if(flag < literals){
		uint2 rng;
		for(int i=0; i<count; i++){
			for(int j=0; j<size; j++){
			    uint randint=MWC64X(&rng);
			    float rand=randint*1.0/BASE;
			        if(flag == 1)
			            printf("randint=%u",randint);
				if(lambdap[j]<rand)
					assignment[lambdas[j]]=0;
				else
					assignment[lambdas[j]]=1;				
			}
		    //the true case
		    assignment[flag]=1;
		    int valuet=0;
		    int index=0;
		    for(int m=0; m<dim1_size; m++){
		    	int valueMono=1;
		    	for(int n=0; n<dim2_size[m]; n++){
		        	if(assignment[lambdas[index+n]]==0){
		        		valueMono=0;
		        		index+=dim2_size[m];
		        		break;
		        	}
		    	}
		    	if(valueMono==1){
		        	valuet=1;
		        	break;
		    	}
		    }        
		    //the false case
		    assignment[flag]=0;
		    int valuef=0;
		    index=0;
		    for(int m=0; m<dim1_size; m++){
		    	int valueMono=1;
		    	for(int n=0; n<dim2_size[m]; n++){
		        	if(assignment[lambdas[index+n]]==0){
		        		valueMono=0;
		        		index+=dim2_size[m];
		        		break;
		        	}
		    	}
		    	if(valueMono==1){
		        	valuef=1;
		        	break;
		    	}
		    }
		    sum += valuet-valuef;            
		}
		influence[flag] = 1.0*sum/count;
		printf("%d:", flag);
		printf("sum=%d",sum);
		printf("%f  ", influence[flag]);
	}
}  


