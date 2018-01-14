
f_f [3] [10        ]
	store s0 0
	store s1 1
	store s2 2
	store s3 3
	store s4 4
	store s5 5
	store s6 6
	store s7 7
	store s8 8
	store s9 9
	s2 = a2
	s1 = a1
	s0 = a0
	s3 = s1
	s4 = s0
	s3 = s3 > s4
	s4 = s1
	s5 = s0
	s4 = s4 == s5
	s3 = s3 || s4
	if s3 == x0 goto l0
	s3 = 0
	a0 = s3
	goto l12
l0:
	s3 = s1
	s4 = s0
	s5 = s3
	a0 = 4
	s5 = s5 * a0
	s5 = s5 + s2
	s5 = s5 [0]
l1:
	s6 = s3
	s7 = s4
	s6 = s6 != s7
	if s6 == x0 goto l2
l3:
	s6 = s3
	s7 = s4
	s6 = s6 < s7
	s7 = s5
	s8 = s4
	a0 = 4
	s8 = s8 * a0
	s8 = s8 + s2
	s8 = s8 [0]
	s7 = s7 < s8
	s8 = s5
	s9 = s4
	a0 = 4
	s9 = s9 * a0
	s9 = s9 + s2
	s9 = s9 [0]
	s8 = s8 == s9
	s7 = s7 || s8
	s6 = s6 && s7
	if s6 == x0 goto l4
	s6 = s4
	s7 = 1
	s6 = s6 - s7
	s4 = s6
	goto l3
l4:
l5:
	s6 = s3
	s7 = s4
	s6 = s6 < s7
	s7 = s5
	s8 = s3
	a0 = 4
	s8 = s8 * a0
	s8 = s8 + s2
	s8 = s8 [0]
	s7 = s7 > s8
	s8 = s5
	s9 = s3
	a0 = 4
	s9 = s9 * a0
	s9 = s9 + s2
	s9 = s9 [0]
	s8 = s8 == s9
	s7 = s7 || s8
	s6 = s6 && s7
	if s6 == x0 goto l6
	s6 = s3
	s7 = 1
	s6 = s6 + s7
	s3 = s6
	goto l5
l6:
	s6 = s3
	s7 = s4
	s6 = s6 < s7
	if s6 == x0 goto l7
	s6 = s3
	a0 = 4
	s6 = s6 * a0
	s6 = s6 + s2
	s6 = s6 [0]
	s7 = s4
	a0 = 4
	s7 = s7 * a0
	s7 = s7 + s2
	s7 = s7 [0]
	s8 = s3
	a0 = 4
	s8 = s8 * a0
	s8 = s8 + s2
	s8 [0] = s7
	s7 = s4
	a0 = 4
	s7 = s7 * a0
	s7 = s7 + s2
	s7 [0] = s6
l7:
	goto l1
l2:
	s4 = s3
	a0 = 4
	s4 = s4 * a0
	s4 = s4 + s2
	s4 = s4 [0]
	s6 = s1
	a0 = 4
	s6 = s6 * a0
	s6 = s6 + s2
	s6 [0] = s4
	s4 = s5
	s5 = s3
	a0 = 4
	s5 = s5 * a0
	s5 = s5 + s2
	s5 [0] = s4
	s4 = s3
	s5 = 1
	s4 = s4 - s5
	s5 = 1
	s3 = s3 + s5
	a0 = s4
	a1 = s1
	a2 = s2
	call f_f
	a0 = s0
	a1 = s3
	a2 = s2
	call f_f
	s0 = 0
	a0 = s0
l12:
	load 9 s9
	load 8 s8
	load 7 s7
	load 6 s6
	load 5 s5
	load 4 s4
	load 3 s3
	load 2 s2
	load 1 s1
	load 0 s0
	return
end f_f
