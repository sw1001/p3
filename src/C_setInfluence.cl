/* 
 * File:   C_setInfluence.cl
 * Author: chenyuan
 *
 * Created on July 22, 2019
 */


#define N 50 // N > index, which is the total number of literals
#define BASE 4294967296UL
//! Represents the state of a particular generator
typedef struct{ uint x; uint c; } mwc64x_state_t;

enum{ MWC64X_A = 4294883355U };
enum{ MWC64X_M = 18446383549859758079UL };

void MWC64X_Step(mwc64x_state_t *s)
{
	uint X=s->x, C=s->c;
	
	uint Xn=MWC64X_A*X+C;
	uint carry=(uint)(Xn<C);				// The (Xn<C) will be zero or one for scalar
	uint Cn=mad_hi(MWC64X_A,X,carry);  
	
	s->x=Xn;
	s->c=Cn;
}

//! Return a 32-bit integer in the range [0..2^32)
uint MWC64X_NextUint(mwc64x_state_t *s)
{
	uint res=s->x ^ s->c;
	MWC64X_Step(s);
	return res;
}



__kernel void setInfluence(const int literals, const int size, const int dim1_size, __global int* lambdas, __global float* lambdap, __global int* dim2_size, __global float* influence){   
	int flag=get_global_id(0);
	int sum=0;
	int count=10000;
	int assignment[N];
    //or try to get newlambda like original version does
    if(flag < literals){
		mwc64x_state_t rng;
		for(int i=0; i<count; i++){
			for(int j=0; j<size; j++){
			    float rand=MWC64X_NextUint(&rng)*1.0/BASE;
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
		printf("%f  ", influence[flag]);
	}
}  


