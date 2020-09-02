void delay(int t)//seconds
{
    t = t*1000;
    usleep(t);
}

int minimum(int *p,int k)
{
    int in = 0;
    int m = p[in];
    for(b = 0; b < k; b++)
    {
        if(p[b]<=m)
        {
            m = p[b];
            in = b;
        }
    }
    return in;
}

int maximum(int *p,int k)
{
    int in = 0;
    int m = p[in];
    for(b = 0; b < k; b++)
    {
        if(p[b]>=m)
        {
            m = p[b];
            in = b;
        }
    }
    return in;
}

int equality(int a, int b, int c)
{
    if(a == b)
    {
        if(b == c)
        {
            return 1;
        }
    }
    return 0;
}

int findmode(int *p,int count_steps)
{
    int mode,count_mode,prev_count_mode;
    int i,j;

    for(i=0;i<count_steps;i++)
    {
        count_mode = 0;
        for(j = i;j<count_steps;j++)
        {
            if(p[i] == p[j])
            {
                count_mode ++;
            }
        }
        if(count_mode !=0 && count_mode > prev_count_mode)
        {
            mode = p[i];
        }
        prev_count_mode = count_mode;
    }

    return mode;
}

int findmean(int *p,int count_steps)
{
    int i,sum=0;
    for(i=0;i<count_steps;i++)
    {
        sum += p[i];
    }
    return sum/count_steps;
}

void decision_algorithm(bool door, bool stair, bool human, bool ground)
{
    if(door == true)
    {
        cout<<"Door"<<endl;
        if(door_toggledetect == 1 && door_prevtoggledetect == 0)
            doordetect(1);
    }
    else if(door == false)
    {
        cout<<"No Door"<<endl;
        if(door_toggledetect == 0 && door_prevtoggledetect == 1)
            doordetect(0);
    }

    if(stair == true)
    {
        cout<<"Stair"<<endl;
        if(stair_toggledetect == 1 && stair_prevtoggledetect == 0)
            stairdetect(1);
    }
    else if(door == false)
    {
        cout<<"No Door"<<endl;
        if(stair_toggledetect == 0 && stair_prevtoggledetect == 1)
            stairdetect(0);
    }

    if(human == true)
    {
        cout<<"Human"<<endl;
        if(human_toggledetect == 1 && human_prevtoggledetect == 0)
            humandetect(1);
    }
    else if(human == false)
    {
        cout<<"No Human"<<endl;
        if(human_toggledetect == 0 && human_prevtoggledetect == 1)
            humandetect(0);
    }

    if(ground == true)
    {
        cout<<"Ground"<<endl;
    }
    else if(ground == false)
    {
        cout<<"No Ground"<<endl;
    }

    human_prevtoggledetect = human_toggledetect;
    door_prevtoggledetect = door_toggledetect;
    stair_prevtoggledetect = stair_toggledetect;
}
