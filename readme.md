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