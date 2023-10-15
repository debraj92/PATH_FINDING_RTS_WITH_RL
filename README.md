# Strategic navigation with Reinforcement Learning in adversarial games

### The agent's objective is to travel from a starting location to a destination through a map populated with enemies - the fire monsters.
### The enemies chase the agent and attempt to damage it.
### If two enemies collide they destroy each other
### The agent cannot damage the enemies. 
### The agent learns strategies which helps it to succeed - for example - the agent can trick the enemies into colliding with each other.
### The overall objective for the agent is to move in a way that keeps deceiving the enemies and thereby avoiding collision. Furthermore, the agent must learn to avoid encirclement by the multiple enemies which is a guaranteed death for the agent if it occurs.
### We have shown in our research that this idea is generalizable. That is, given a different game with varying rules, the agent can autonomously learn strategies specific to that game which enables it to perform adversarial navigation.
### We have also shown that our idea is scalable. That is, the agent can navigate maps of large sizes even though they are trained on small maps.
### Furthermore, the agent can navigate game maps that are significantly different from the training maps because of abstractions that we introduced in our research

Here are two short demoes of the agent navigating in adversarial environments. The starting location is indicated by a ring and the destination is marked by a flag.

![demo](https://user-images.githubusercontent.com/10151782/195716974-41fd1bdf-ea26-4094-a3b7-59100ca261a7.gif)


![ezgif com-video-to-gif](https://github.com/debraj92/PATH_FINDING_RTS_WITH_RL/assets/10151782/0908ebb4-b13d-404a-8952-4a57f497fb69)

## The reseach is published in AIIDE / AAAI
https://ojs.aaai.org/index.php/AIIDE/article/view/27530

