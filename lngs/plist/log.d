==11477== Memcheck, a memory error detector
==11477== Copyright (C) 2002-2010, and GNU GPL'd, by Julian Seward et al.
==11477== Using Valgrind-3.6.1 and LibVEX; rerun with -h for copyright info
==11477== Command: ./intle in.d
==11477== Parent PID: 2691
==11477== 
==11477== 
==11477== HEAP SUMMARY:
==11477==     in use at exit: 298,947 bytes in 46,940 blocks
==11477==   total heap usage: 55,339 allocs, 8,399 frees, 448,159 bytes allocated
==11477== 
==11477== LEAK SUMMARY:
==11477==    definitely lost: 78,762 bytes in 36,503 blocks
==11477==    indirectly lost: 60,842 bytes in 2,970 blocks
==11477==      possibly lost: 0 bytes in 0 blocks
==11477==    still reachable: 159,343 bytes in 7,467 blocks
==11477==         suppressed: 0 bytes in 0 blocks
==11477== Rerun with --leak-check=full to see details of leaked memory
==11477== 
==11477== For counts of detected and suppressed errors, rerun with: -v
==11477== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 4 from 4)
