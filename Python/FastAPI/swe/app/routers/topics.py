from fastapi import APIRouter, HTTPException, Depends, status
from datetime import datetime, timedelta
from app.models import db, Topic
from app.schemas import TopicCreate, TopicOut
from app.dependencies import get_current_user

router = APIRouter(prefix="/topics", tags=["Topics"])


# --- Helpers ---

def is_expired(topic: Topic) -> bool:
    return datetime.utcnow() > topic.ttl

def purge_expired_topics():
    expired = [tid for tid, t in db["topics"].items() if is_expired(t)]
    for tid in expired:
        # Remove associated comments
        db["comments"] = {
            cid: c for cid, c in db["comments"].items()
            if c.topic_id != tid
        }
        del db["topics"][tid]


# --- Create Topic ---

@router.post("/", response_model=TopicOut, status_code=status.HTTP_201_CREATED)
def create_topic(payload: TopicCreate, current_user=Depends(get_current_user)):
    purge_expired_topics()

    content_hash = hash(payload.content.strip().lower())
    for topic in db["topics"].values():
        if topic.hash == content_hash:
            raise HTTPException(
                status_code=status.HTTP_409_CONFLICT,
                detail="Topic already exists"
            )

    new_topic = Topic(
        owner_id=current_user.id,
        content=payload.content,
        hash=content_hash,
        ttl=datetime.utcnow() + timedelta(hours=1),
        current_comment_number=0,
        previous_comment_number=0,
        total_comment_number=0
    )
    db["topics"][new_topic.id] = new_topic
    return new_topic


# --- List Topics ---

@router.get("/", response_model=list[TopicOut])
def list_topics(current_user=Depends(get_current_user)):
    purge_expired_topics()
    return list(db["topics"].values())


# --- Get Topic ---

@router.get("/{topic_id}", response_model=TopicOut)
def get_topic(topic_id: str, current_user=Depends(get_current_user)):
    purge_expired_topics()

    from uuid import UUID
    try:
        uid = UUID(topic_id)
    except ValueError:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail="Invalid UUID")

    topic = db["topics"].get(uid)
    if not topic:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Topic not found")

    return topic
