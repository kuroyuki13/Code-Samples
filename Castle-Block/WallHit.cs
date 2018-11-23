using UnityEngine;
using System.Collections;

public class WallHit : MonoBehaviour {

    public bool m_DebugMode, isFoundation;

    public Player m_Player = Player.None;
    private GameManager m_GameManager;
    private GameObject m_Audio;

    
    GameObject Wall;
    Rigidbody rb;
    RaycastHit hit;
    Component[] list;
    Vector3 testpos;
    Vector3 direction;
    float damage;
    float Health;
    float blockdistance;
    bool destroyed;
    bool isAnimating;
    PhysicMaterial mat;
    
    private void Awake()
    {
       m_GameManager = FindObjectOfType<GameManager>();
        m_Audio = GameObject.FindGameObjectWithTag("AudioManager");
    }

    // Use this for initialization
    void Start () {
       
        mat = m_GameManager.BlockMaterial;
        
        Wall = this.gameObject;
        rb = Wall.GetComponent<Rigidbody>();

        if (isFoundation)
        {
            Health = m_GameManager.Foundation_Health;
        }
        else
        {
            Health = m_GameManager.Normal_Health;
        }
        damage = m_GameManager.Ammo_Dammage;
        destroyed = false;
        rb.freezeRotation = true;
        //list = Wall.GetComponentsInChildren<BoxCollider>();
        //foreach(BoxCollider i in list)

        list = Wall.GetComponentsInChildren<MeshCollider>();
        foreach(MeshCollider i in list)
        {
            i.material = mat;
        }
        Transform test;
        test = Wall.transform.FindChild("testpos");
        
        if (test != null)
        {
            testpos = test.transform.position;
        }
        else
        {
            testpos = Wall.transform.position;
        }
        isAnimating = false;
        Wall.AddComponent<Animator>();
        Wall.GetComponent<Animator>().runtimeAnimatorController = m_GameManager.AnimController;
    }
	
	// Update is called once per frame
	void Update () {

        //check if block is still supported, otherwise is not kinematic
        if (Physics.Raycast(Wall.transform.position, Vector3.down, out hit))
        {
            if (hit.distance > blockdistance+0.1f) rb.isKinematic = false;
        }
        
        // if block is destroyed, remove kinematic, and add force
        if (Health <= 0 && !destroyed)
        {
            m_Audio.GetComponent<AudioManager>().PlayClip(audioClip.Crumble);
            destroyed = true;
            rb.isKinematic = false;
            float dir = Wall.transform.position.x - Wall.transform.root.position.x;
            if (dir > 0) dir = 1;
            else if (dir < 0) dir = -1;
           // float dir = 0;
            //if (m_Player == Player.Left) dir = -1;
            //else if (m_Player == Player.Right) dir = 1;
            rb.freezeRotation = false;
            bool canMove = false;
            // check raycast upwards to see if there is block above it
            if (!Physics.Raycast(testpos, Vector3.up, 1))
            {
                // if no block above it remove physics material to let it topple better.
                //foreach(BoxCollider i in list)

                foreach (MeshCollider i in list)
                {
                    i.material = null;
                }
                canMove = true;
            }
            // adding the force to move it.
            if (!isFoundation || (isFoundation && canMove))
            {
                Wall.GetComponent<Rigidbody>().AddForce(Vector3.right * dir * 100);
            }         
        }
        if (isFoundation && destroyed)
        {
            if (!Physics.Raycast(testpos, Vector3.up, 1))
            {
                Destroy(Wall); //the following code is never triggered because with particles, fading is no longer required
                isAnimating = true;
                Wall.GetComponent<Animator>().SetTrigger("FloorDetected");
            }
        }
        if (isAnimating)
        {
           if ( Wall.GetComponent<Animator>().GetCurrentAnimatorStateInfo(0).IsTag("done"))
           {
                Destroy(Wall);
           }
        }

      
	
	}
    private void OnCollisionEnter(Collision collision)
    {
        
        // check if the collided object is ammunition and do damage if true. destroy ammo
        if (collision.gameObject.tag == "ammo" && !destroyed)
        {
            if ((m_Player == Player.Left && collision.gameObject.GetComponent<CharacterModelLoader>().m_Faction == CharacterFaction.Bird) ||
                (m_Player == Player.Right && collision.gameObject.GetComponent<CharacterModelLoader>().m_Faction == CharacterFaction.Human))
            {
                Health -= damage;
                
                if (m_DebugMode)
                {
                    Debug.Log(gameObject.name + ": " + Health);
                }

                switch (m_Player)
                {
                    case (Player.Left):
                        Instantiate(m_GameManager.m_PlayerBlood, collision.transform.position, new Quaternion(0, 90, 0, 0)); //blood particles are spawned at the character's position
                        Instantiate(m_GameManager.m_HouseDebris, collision.transform.position, new Quaternion(0, 0, 0, 0)); //debrif particles are spawned whenever the player hits a block
                        break;
                    case (Player.Right):
                        Instantiate(m_GameManager.m_PlayerBlood, collision.transform.position, new Quaternion(0, 0, 0, 0)); //blood particles are spawned at the character's position
                        Instantiate(m_GameManager.m_HouseDebris, collision.transform.position, new Quaternion(0, 180, 0, 0)); //debrif particles are spawned whenever the player hits a block
                        break;
                }
                

                Destroy(collision.gameObject);
            }
            m_Audio.GetComponent<AudioManager>().PlayClip(audioClip.Wallhit);

        }
        //check if colliding object is a wall and not destroyed,
        // if so  set to kinematic and record distance. works as intended as if now, but has potential for issues.
        if (collision.gameObject.tag == "wall" && !destroyed)
        {
            Physics.Raycast(Wall.transform.position, Vector3.down, out hit);
            blockdistance = hit.distance;

            rb.isKinematic = true;
        }

        if (collision.gameObject.tag == "floor" && destroyed)
        {
            m_Audio.GetComponent<AudioManager>().PlayClip(audioClip.RockGround);
            Instantiate(m_GameManager.m_HouseExplosion, collision.contacts[0].otherCollider.transform.position, new Quaternion(0, 0, 0, 0)); //an explosion particle play when a block runs out of health
            Destroy(Wall); //the following code is never triggered because with particles, fading is no longer required
            isAnimating = true;
            Wall.GetComponent<Animator>().SetTrigger("FloorDetected");
        }
    }
    private void OnCollisionStay(Collision collision)
    {
        if (collision.gameObject.tag == "floor" && destroyed)
        {
            m_Audio.GetComponent<AudioManager>().PlayClip(audioClip.RockGround);
            Instantiate(m_GameManager.m_HouseExplosion, collision.contacts[0].otherCollider.transform.position, new Quaternion(0, 0, 0, 0)); //an explosion particle play when a block runs out of health
            Destroy(Wall); //the following code is never triggered because with particles, fading is no longer required
            isAnimating = true;
            Wall.GetComponent<Animator>().SetTrigger("FloorDetected");
        }
    }

    private void OnEnable()
    {
        m_GameManager.AddBlock(m_Player);
    }

    private void OnDisable()
    {
        m_GameManager.RemoveBlock(m_Player);
    }
}
