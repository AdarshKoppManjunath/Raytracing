# Raytracing
<br>The objective is to write a ray-tracing program in sequential and then parallelizing the same using OpenMP. Also, to calculate the running time of both the programs and compare the same.

<br>Both sequential and parallel are written in C++, and executed and tested on Cygwin
<br>with GCC compiler and OpenMP. This document will help in understanding how to
<br>execute programs, the output of both sequential and parallel, time taken by both the
<br>programs, and comparison between them.
<br><br><b>1) Initial Setup</b>
<br>          • Install Cygwin on windows system from https://www.cygwin.com/
<br>• During installation, select “ http://cygwin.mirros.hoobly.com ” as a
download site.
<br>• In the packages, select OpenMP, C, and GCC related packages.
<br>• Finish installation.
<br>• After installation, we should be able to see the g++ version on the Cygwin
command prompt
    <img src="https://github.com/AdarshKoppManjunath/Raytracing/blob/master/Screenshots/Fig1.PNG" alt="Smiley face" >

<br><br> Should be able to see below folders under “cygwin/lib”
 <br><img src="https://github.com/AdarshKoppManjunath/Raytracing/blob/master/Screenshots/Fig2.PNG" alt="Smiley face" >
 
 <br><br><b>
2) Execution:</b>
<br>• Copy the CS802-PA-AdarshKoppaManjunath-200397257 folder to a
directory and open this directory in the cygwin command prompt.
<br>• We have “SequentialRaytracing.cpp”, “ParllelRaytracingWithOpenMP.cpp”
and “geometry.h” files
<br>• “geometry.h” consists of all vectors operations like addition, subtraction,
multiplication, and division. We need this file to run both sequential and
parallel codes.
<br>• To compile squential code, type “g++ SequentialRaytracing.cpp -o
SequentialRaytracing” on cygwin command prompt.
<br>• After compilation, execute the same with the command “time
./SequentialRatracing”
<br>• We can see the sequential ray tracing output in the below snippet.
<br><img src="https://github.com/AdarshKoppManjunath/Raytracing/blob/master/Screenshots/Fig3.PNG" alt="Smiley face" >
<br>• From the above snippet, we can see it took 43.26 seconds to complete
sequential raytracing algorithm (this doesn’t include initial setup or image
array copying to .ppm file time. This is Purely time taken to complete ray
tracing algorithm sequentially) . This time is calculated by referring the legacy
code given with the assignment.
<br>• time command while running the program also gives real, user, and sys time.
Real time is the actual time taken from start to end. User time is the cumulative
time spent by all the CPUs during the computation and sys time is the
cumulative time spent by all the CPUs during system-related tasks such as
memory allocation.
<br>• On executing it also generates the “SequentialRaytracing.ppm” file. Which
can be viewed and converted to jpeg on “https://convertio.co/ppm-jpg/”
<br><img src="https://github.com/AdarshKoppManjunath/Raytracing/blob/master/Screenshots/Fig4.PNG" alt="Smiley face" >

<br><br>• To execute “ParallelRaytraceWithOpenMp.cpp”, we need to pass -fopenmp
command while compiling.
“g++ -fopenmp ParallelRaytracingWithOpenMP.cpp -o ParallelRaytracing
<br>• After compling we can run the same using “time
./ParallelRaytracingWithOpenMP” command
<br><img src="https://github.com/AdarshKoppManjunath/Raytracing/blob/master/Screenshots/Fig5.PNG" alt="Smiley face" >
<br>• From the above snippet, we can see it took 13.0064 seconds to complete
parallel raytracing algorithm (this doesn’t include initial setup or image array
copying to .ppm file time. This is Purely time taken to complete ray tracing
algorithm parallely). This time is calculated by referring the legacy code given
with the assignment.
<br>• On executing it also generates the “SequentialRaytracing.ppm” file. Which
can be viewed and converted to jpeg on “https://convertio.co/ppm-jpg/”
<br><img src="https://github.com/AdarshKoppManjunath/Raytracing/blob/master/Screenshots/Fig6.PNG" alt="Smiley face" >

<br><br><b>
3) Comparison</b>
<br>• Raytracing Algorithm consists of a lot of computation. Concepts like
diffusion, specular, shadow, reflection, recursion, intersection, materials, etc.
takes a lot of time in terms of computation. As computation increases, time
consumption also increases. In this case, we can get benefited by parallel
programming models like OpenMP which would run computations in parallel
and finishes tasks early.
<br>• A parallel method is useful for a complex and larger problem, compared to a
smaller problem. With my current knowledge in ray tracing, a sequential
program that I have modified has very little computation and it took
around 43.26 seconds to finish the ray tracing algorithm. With the help
of OpenMP, I achieved the same task in 13.006 seconds. Which gives 
speedup of = 43.26 / 13.006 = 3.32. Number of threads=4, therefore
efficiency = 3.32/4 *100 = 83%
<br>• As computation increases ( for example, on adding more objects, increasing
the size of objects, increasing the reflection depth, increasing the scene
(image) size, or with more lightning more operation) ray tracing algorithm
will take way more time to execute sequentially. And parallel time will
increase only by a small value.

<br><br><b>
References:</b>
<br>1) S. M. Ashraful Kadir and Tazrian Khan. Parallel Ray Tracing using MPI and
OpenMP.2008.https://www.researchgate.net/publication/228686336_Paralle
l_Ray_Tracing_using_MPI_and_OpenMP
<br>2) https://github.com/ssloy
<br>3) https://code.google.com/archive/p/simple-ray-tracing
<br>4) http://kylehalladay.com/blog/tutorial/math/2013/12/24/Ray-SphereIntersection.html
<br>5) https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-toshading/reflection-refraction-fresnel
<br>6) https://github.com/marczych/RayTracer
    
    
 



