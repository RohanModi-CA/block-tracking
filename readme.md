# block-tracking


So we're going to try to track the position of the centre of this block from a video. The first step is to get each of the frames. We'll do that later. For now, we'll assume we have the frames. We'll deal with the ensemble later as well. 

First step, look at an individual frame. We'll assume we have it in ppm format, because that's the easiest to deal with. Normally I would just write my own ppm parser. In fact I might still do that. But I need to stop the NMH syndrome so I'll spend a few minutes just to see whether something suitable exists and works!

Okay, [this](https://github.com/leimao/PPMIO) seems to work. Take that, NMHS! Not so fast, this is pure cpp without any extern C or anything so I don't think I can actually use this. Liars.

Okay to be honest I didn't find much. I'll check just a bit more. Oh, actually maybe I can just use the one I used like two years ago! I don't know where I found this but thank you "James Massengill".
[Here](https://github.com/RohanModi-CA/OPS/blob/f6b82576a8c96b3c0d6d4d8cebc0a5cb9e1e3a65/lib/ppm_lib/ppm.c).


---

Okay took a second to re-figure this out but very cool.


Normalizing the colors
---

Okay, we've loaded in the image. Now what's our first question? I think what we generally want to be able to do is to get the file to a point where every pixel is one color. We need some rules on this. I think the first thing we could try is purely a "what's closest" approach and see how that goes.

We'll do this in a file meant for it. Okay, done. Let's see what's next. Also maybe we can replace the PPM with a funky-ppm after normaling that stores enum indices instead of RGBs but that's for another time (or never).

Hmm. Screwholes are likely to be an issue. 

Anyway, here is the result from least squares:

![](attachments/Pasted%20image%2020260204234215.png)



I wonder if we could do better by setting white to be grey. 

![](attachments/Pasted%20image%2020260204234316.png)

Helps a fair bit. I guess the shadow is a problem though that we can't really get around easily since there is no reason why locally we would expect that shadow to be 'white'. 

One approach could be to make a 'background' color parameter and try to build up some shadow logic: seems annoying and probably not adaptable to other issues we might have.

We could try to define a "shadow" color and handle that; might work.

We could try to "just have better lighting". That's a great start, to be honest. Actually, since we're going to be putting this against a black background, maybe shadows aren't actually a big issue at all. That is great! 

![](attachments/Pasted%20image%2020260204235550.png)

Hmm.. Well, garbage in, garbage out. I trust (hope) we'll have better contrast on the setup, But looking at this, we note that it actually might not be so hard to conceive of a working method to find the center of the blue:

![](attachments/Pasted%20image%2020260204235818.png)

Since even though there is all that blue scattered about the background, etc, it is enclosed by green. Maybe with some minimum area requirements, it won't be that bad. Another issue is that we note that the center of mass isn't actually in the centre of this blue rhombus, it is to the left of it, since the block is tilted. Is this an issue for finding the COM for us? Ours (hopefully) won't be tilted, but it won't be in the centre of the camera. I think? if it is not tilted then this is not an issue? 


Hitboxing
---

So if we're satisfied with the above, which for now we are, we have the main thing to worry about: how do we know where the blue is? 

So one idea is gridding and just summing the counts in each gridbox:

![](attachments/Pasted%20image%2020260205005304.png)
The issue with this is of course that we're not really sure what to do when it crosses a grid border. In the shower I thought about sort of 'subgridding' further and then using this to identify border-crossers, but I think its a bit finnicky. Not a huge fan.

However, thinking about and reading about other approaches seems equally unpleasant. Lots of filtering and dealing with connected components, which, by the way, isn't ideal for us since the connectedness isn't great for our concentric boxes.

Hmm. Doing research on this, seems complicated. One thing that seems to work is doing a course-grained average on the image, and then thresholding it. 

In reading about this I learned of [summed area tables]([https://www.cis.rit.edu/~rlepci/Erho/Derek/Useful_References/Miscellaneous/SATs_1.pdf](https://en.wikipedia.org/wiki/Summed-area_table)), which means that at least we won't have to do a bunch of averaging for each pixel. I think we'll just implement this first.

I think we can make some rules for the summed area table. For this I think it's best to really stop dealing with RBG ppm's so let's just define our own small PPMs which we can eventually go back and use with the enum thing.

We'll make it have an array of uchars, since 255 is way more than enough values. And I guess we'll need a map from the uchars back to RBGs, although this is optional, I guess. Just to make it easier on us. Okay cool done, now let's just make the summed table from small PPMs.

Oh wait this doesn't work because the sums can themselves get large. I guess let's make them ints. What a waste! Also I recognzie we could technically overflow this but we'll only be using like 4 colors so it won't happen. However we could actually overflow unsigned char so let's be safe.

Okay now let's check that the rewards thing works correctly. We're going to use the rewards to try to binarize. Maybe let's look at the enum thing.

Okay, no enum thing, which is unncessary, we just made a greyscale map. Anyway, we can binarize very well using this, which makes sense because it is a very simple task. Anyway, here is binarizing to reward only the Blue, which naturally just extracts the blue:

![](attachments/Pasted%20image%2020260206133031.png)

Note that since our picture had a lot of blue, it's not great, but this is something we can deal with.

We can then do a basic summed area table based on this, which looks just as we might expect:

![](attachments/Pasted%20image%2020260206133136.png)

Not much to see; we have to do a density map on this to smooth things out and recover a smoothed version of the original. What happens if we penalize green? 

![](attachments/Pasted%20image%2020260206133347.png)
That's interesting, though I'm not really sure how useful it is. Anyway, let's just density map smooth this thing out first and then we'll see. 

Turns out intensity is nonlinear so I'll just fix my visualization to see if that helps anything, apparently we can write 

$$I=255(0.1p+0.3p^2+0.6p^3)$$
to get RGB intensity from percent brightness. This doens't affect the block tracking at all its just for me to get better intuition.

![](attachments/Pasted%20image%2020260206135842.png)
Hmm. Not sure what to make of that but it does seem that there is more information there? 

Okay, anyway, let's just make a density mapping (I think this just ends up being a box blur).

This is for the heavily penalized one:
![](attachments/Pasted%20image%2020260206145239.png)
Okay, cool.  Not sure what to make of this but we'll think about it. Then let's just do the blue unpenalized.

![](attachments/Pasted%20image%2020260206145343.png)

Hmm. That's.. not right? Oh wait maybe it's just a rounding error. Okay, fixed it, but this isn't much better:
![](attachments/Pasted%20image%2020260206150407.png)

The blue is completely black for some reason, and solitary pixels create massive blocks. Something is off.

```c
min_pixel = this_pixel < min_pixel ? this_pixel : min_pixel;
max_pixel = this_pixel > min_pixel ? this_pixel : max_pixel;
```

*Genius*, Rohan. 

![](attachments/Pasted%20image%2020260206154821.png)
So much better. So this is what the unpenalized blue looks like, let's check green:

![](attachments/Pasted%20image%2020260206154935.png)
Cool, maybe the radius is too big, but whatever. What if we harshly penalize green?

![](attachments/Pasted%20image%2020260206155159.png)

This is kind of good: white surrounded by black, should be possible to extract. Let me try with a smaller radius, of 10:

![](attachments/Pasted%20image%2020260206155311.png)
That could be good. I think we can threshold this and then look for white enclosed within black? 

![](attachments/Pasted%20image%2020260206155724.png)
That took a long time (about a second .. yikes hopefully with compiler optimization and less back and forth conversion/writes it will be okay). Actually all the writing could actually be a part of that so I won't worry too much time at this point, but I also won't check because blissful ignorance is good sometimes.

Also, I think we got lucky on this with the tape not being there. But then again we shoudn't have tape in the real image? Well anyway we have the black which is important.

Regardless, now we need to find just the white pixels within the black. And then hopefully we can fit a trapezoid to it. 

So how do we identify white within black in O(N) time? [LLMs](https://claude.ai/share/257500da-07ce-43d4-ad85-ca12d61dc1f5) made a good suggestion, to discover the regions, we can just iterate through checking if its neighbors are white and use this to build a region (called a flood fill), and then check if the white ever touches the edge of the image, meaning it is not surrounded by black. And then we can just check the area of the region to ensure that it is in reasonable limits. Very cool. 

Hmm. Actually we may have run into an issue, non-blue is also white. Is that an issue? hmm. Well what do we want? If there is blue touching a pixel of red, should the region be discarded? After thresholding, I'm inclined to say, yes. 

I think we lost sight of what we were doing, the whole density thing was mostly just a blur. So we'll have to clean this up, but I see how that is done: fix rewards to penalize other colors by other amounts, then the density map is just an an averager, so that's fine, and then we just threshold to the original colors. What we've done is just made the blur to avoid indivual pixels triggering as something else and breaking the regions. Okay, so we're on track. 

Anyway back to flood fill. We'll track the boundary colors. Perhaps we could have some tolerance (1 red pixel is fine?). I think it's a good way to go.

flood fill
---

Okay so the recursive call needs to know: where to start index, visited_yet so it can keep calling, and some sort of list of information about its region boundaries. This.. could be a large list. Well, no, since we're asking the color of the region boundaries, not the ID. So with 4 colors its a small list.  Also I think we'll convert to 2d first because this seems annoying otherwise. Actually not really.

So the recursive call gets a pixel and the current region color. It checks if it has been visited already. If so, return. If not, it should check whether this pixel is of the correct color. If so, mark this pixel as visited, and call the recursive call on all of your neighbours. If not, you've reached a new region, at which point you should:

1. Add this color to the region boundaries of the old region
2. Mark yourself as visited
3. Call yourself on your neighbours with ++region_number and the new color.

So outside the flood fill, we will handle multiple regions. 

AND, if you don't touch the edge and you ONLY have one color that borders you, you are surrounded by that color! 

I think we got it.

