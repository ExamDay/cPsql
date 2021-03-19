from rockstar import RockStar

code = "printf('Hello, world!');"
rock_it_bro = RockStar(offset=612, days=34, file_name='main.c', off_fraction=0.34, code=code)
#  rock_it_bro = RockStar(days=820)
rock_it_bro.make_me_a_rockstar()
