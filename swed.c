
int main_edit_loop(void){   
   FILE *infile;
   int xp,yp,lev,im;
   GraphicsContext realscreen;
   GraphicsContext virtualscreen;
   int current_block;
   initialise();
   load_graphics();
   infile = fopen("levelshi","rb");
   for(lev=0;lev<20;lev++) {
      for(xp = 0; xp < 64; xp++) {
	 for(yp = 0; yp <48; yp++) {
	    fread(&(levels[lev][xp][yp]),1,1,infile);
	 }
      }
   }
   fclose(infile);
   
   xp = yp = 0;
   
   keyboard_init();
   gl_setcontextvga(10);
   gl_getcontext( &realscreen );
   gl_setcontextvgavirtual(10);
   gl_getcontext( &virtualscreen );
   current_block = 0;
   while(1) {
      keyboard_update();
      if(keyboard_keypressed(SCANCODE_Q)) {
	 yp--;
	 if(yp<0) {
	    yp=0;
	 }
      }
      if(keyboard_keypressed(SCANCODE_A)) {
	 yp++;
	 if(yp>47) {
	    yp=47;
	 }
      }
      if(keyboard_keypressed(SCANCODE_O)) {
	 xp--;
	 if(xp<0) {
	    xp = 0;
	 }
      }
      if(keyboard_keypressed(SCANCODE_P)) {
	 xp++;
	 if(xp>63) {
	    xp = 63;
	 }
      }
      if(keyboard_keypressed(SCANCODE_CURSORBLOCKUP)) {
	 current_block+=4;
	 if(current_block > 255) {
	    current_block = 0;
	 }
      }
      if(keyboard_keypressed(SCANCODE_CURSORBLOCKDOWN)) {
	 current_block-=4;
	 if(current_block < 0) {
	    current_block = 0;
	 }
      }
      if(keyboard_keypressed(SCANCODE_CURSORUP)) {
	 current_level++;
	 if(current_level > 19) {
	    current_level = 19;
	 }
      }
      if(keyboard_keypressed(SCANCODE_CURSORDOWN)) {
	 current_level--;
	 if(current_level < 0) {
	    current_level = 0;
	 }
      }
      if(keyboard_keypressed(SCANCODE_SPACE)) {
	 levels[current_level][xp][yp] = current_block;
      }
      if(keyboard_keypressed(SCANCODE_ESCAPE)) {
	 infile = fopen("levelshi","wb");
	 for(lev=0;lev<20;lev++) {
	    for(xp = 0;xp < 64; xp++) {
	       for(yp = 0; yp <48; yp++) {
		  if(levels[lev][xp][yp] == 64) {
		     levels[lev][xp][yp] = 4;
		  }
		  fwrite(&(levels[lev][xp][yp]),1,1,infile);
	       }
	    }
	 }
	 fclose(infile);
	 keyboard_close();
	 exit(1);
      }
      draw_screen(xp,yp);
      gl_putboxmask(320,224,32,32,graphics[current_block]);
      gl_copyscreen(&realscreen);
   }
   keyboard_close();
   return EXIT_SUCCESS;
}
   
